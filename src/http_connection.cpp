#include <string>
#include <fstream>
#include <new>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "http_connection.h"
#include "easylogging++.h"

namespace thanos {

// register prototype to Connection
HTTPConnection HTTPConnection::_http_connection;

std::map<std::string, std::string> HTTPConnection::_mime_type;

HTTPConnection::HTTPConnection() : 
        _request(), _response(), _parse_status(ParseStatus::PARSE_REQUEST_LINE) {
    _add_prototype(this);
}

HTTPConnection::HTTPConnection(int dummy) : 
        _request(), _response(), _parse_status(ParseStatus::PARSE_REQUEST_LINE) {}

HTTPConnection::~HTTPConnection() = default;

Connection* HTTPConnection::_clone() {
    return new HTTPConnection(5);
}

void HTTPConnection::_init_mime() {
    std::string ext_name = "";
    std::string mime_type = "";

    std::stringstream ss;
    std::ifstream mime_file("/home/van/workspace/thanos/mime.txt");
    std::string line = "";

    while (std::getline(mime_file, line)) {
        ss.str(line);
        ss >> mime_type >> ext_name;
        ss.clear();
        _mime_type[ext_name] = mime_type;
    }
}

bool HTTPConnection::_clear() {
    _request.clear();
    _response.clear();
    _parse_status = ParseStatus::PARSE_REQUEST_LINE;    
    return true;
}

void HTTPConnection::process() {
    Buffer read_buffer;
    Buffer write_buffer;


    // 1、取出读缓存
    if (!_dump_read(&read_buffer) || read_buffer.empty()) {
        _error_response(HTTPCode::HTTP_INTERNAL_ERROR);
        return;
    }

    // DEBUG
    //read_buffer.show_content();

    // 2、解析请求
    _parse_request(read_buffer);
    if (_parse_status == ParseStatus::PARSE_ERROR) {
        _error_response(HTTPCode::HTTP_BAD_REQUEST);
        return;
    }
    if (_parse_status != ParseStatus::PARSE_DONE) {
        // 状态机未达到结束状态，简单返回即可
        return;
    }

    // 3、根据请求方法转到相应的句柄，在相应的句柄中生成响应报文(类形式)
    HTTPCode code = _handle_request();
    if (code != HTTPCode::HTTP_OK) {
        _error_response(code);
        return;
    }

    // 4、将类形式的响应报文转变为字节流形式
    if (!_create_response_stream(&write_buffer)) {
        _error_response(HTTPCode::HTTP_INTERNAL_ERROR);
        return;
    }

    // 5、将局部变量write_buffer移动到成员变量_write_buffer中
    if (!_dump_to_write(write_buffer)) {
        _error_response(HTTPCode::HTTP_INTERNAL_ERROR);
        return;
    }

    // 6、提示Connection处理完毕
    if (!_process_done()) {
        LOG(WARNING) << "[HTTPCOnnection::process]: _process_done failed";
        return;
    }
}

std::string HTTPConnection::_make_status(const HTTPCode& code) {
    switch (code) {
    case HTTPCode::HTTP_OK:
        return "200";
    case HTTPCode::HTTP_BAD_REQUEST:
        return "400";
    case HTTPCode::HTTP_FORBIDDEN:
        return "403";
    case HTTPCode::HTTP_NOT_FOUND:
        return "404";
    case HTTPCode::HTTP_METHOD_NOT_ALLOWED:
        return "405";
    case HTTPCode::HTTP_INTERNAL_ERROR:
        return "500";
    }
}

std::string HTTPConnection::_make_status_info(const HTTPCode& code) {
    switch (code) {
    case HTTPCode::HTTP_OK:
        return "OK";
    case HTTPCode::HTTP_BAD_REQUEST:
        return "Bad Request";
    case HTTPCode::HTTP_FORBIDDEN:
        return "Forbidden";
    case HTTPCode::HTTP_NOT_FOUND:
        return "Not Found";
    case HTTPCode::HTTP_METHOD_NOT_ALLOWED:
        return "Method Not Allowed";
    case HTTPCode::HTTP_INTERNAL_ERROR:
        return "Internal Server Error";
    }
}

void HTTPConnection::_error_response(const HTTPCode& code) {
    Buffer buffer;

    // status line
    std::string status_line = _request.get_version() + " " + _make_status(code) +
            " " + _make_status_info(code) + "\r\n";
    buffer.push_nbytes(status_line.c_str(), status_line.length());

    char buf[1024];
    sprintf(buf, "<html><title>Thanos Error</title>");
    sprintf(buf, "%s<body bgcolor=""ffffff"">\r\n", buf);
    sprintf(buf, "%s%s: %s\r\n", buf, _make_status(code).c_str(), _make_status_info(code).c_str());
    sprintf(buf, "%s<hr><em>The Thanos server</em>\r\n", buf);

    // headers
    std::string header_line = "";
    header_line = "Content-Type:text/html\r\n";
    buffer.push_nbytes(header_line.c_str(), header_line.length());
    header_line = "Content-Length:" + std::to_string(strlen(buf)) + "\r\n";
    buffer.push_nbytes(header_line.c_str(), header_line.length());
    header_line = "\r\n";
    buffer.push_nbytes(header_line.c_str(), header_line.length());

    // body
    buffer.push_nbytes(buf, strlen(buf));

    if (!_dump_to_write(buffer)) {
        LOG(FATAL) << "[HTTPConnection::_error_response]:_dump_to_write failed";
        return;
    }
    if (!_process_done()) {
        LOG(FATAL) << "[HTTPConnection::_error_response]:_dump_to_write failed";
    }
}

bool HTTPConnection::_create_response_stream(Buffer* buffer) {
    if (buffer == nullptr) {
        LOG(WARNING) << "[HTTPConnection::_create_response_stream]: parameter nullptr";
        return false;
    }
    buffer->clear();

    std::string status_line = _response.get_version() + " " + _response.get_status() +
            " " + _response.get_info() + "\r\n";
    buffer->push_nbytes(status_line.c_str(), status_line.length());

    const std::map<std::string, std::string>& headers = _response.get_headers();
    std::string header_line = "";
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        header_line = it->first + ":" + it->second + "\r\n";
        buffer->push_nbytes(header_line.c_str(), header_line.length());
    }

    header_line = "\r\n";
    buffer->push_nbytes(header_line.c_str(), header_line.length());

    if (_response.get_body() == nullptr || _response.get_body_len() == 0) {
        LOG(WARNING) << "[HTTPConnection::_create_response_stream]: response body nullptr";
        return false;
    }

    buffer->push_nbytes(_response.get_body(), _response.get_body_len());

    if (munmap(_response.get_body(), _response.get_body_len()) == -1) {
        LOG(WARNING) << "[HTTPConnection::_create_response_stream]: munmap failed";
        return false;
    }
    _response.set_body(nullptr);
    _response.set_body_len(0);
    return true;
}

void HTTPConnection::_parse_request(Buffer& buffer) {
    std::string line = "";
    BufferReadStatus line_status = BufferReadStatus::READ_SUCCESS; // 从状态机

    while (true) {
        line_status = buffer.get_line(&line);
        if (line_status != BufferReadStatus::READ_SUCCESS && 
                line_status != BufferReadStatus::READ_END) {
            break;
        }

        switch (_parse_status) {
        case ParseStatus::PARSE_REQUEST_LINE:
            if (!_parse_request_line(line)) {
                _parse_status = ParseStatus::PARSE_ERROR;
                return;
            }
            _parse_status = ParseStatus::PARSE_HEADER;
            break;

        case ParseStatus::PARSE_HEADER:
            if (!_parse_header(line)) {
                _parse_status = ParseStatus::PARSE_ERROR;
                return;
            }
            if (line.empty()) {
                _parse_status = ParseStatus::PARSE_BODY;
            }
            break;
        }

        if (_parse_status == ParseStatus::PARSE_BODY) {
            if (!_parse_body(buffer)) {
                LOG(WARNING) << "[HTTPConnection::_parse_request]:parse body failed";
                _parse_status = ParseStatus::PARSE_ERROR;
                return;
            }
            _parse_status = ParseStatus::PARSE_DONE;
        }
        if (_parse_status == ParseStatus::PARSE_DONE) {
            return;
        }
        if (line_status == BufferReadStatus::READ_END) {
            break;
        }
        line.clear();
    }

    if (line_status != BufferReadStatus::READ_END) {
        LOG(WARNING) << "[HTTPConnection::_parse_request]: buffer.get_line failed";
        _parse_status = ParseStatus::PARSE_ERROR;
    }
}

bool HTTPConnection::_parse_request_line(const std::string& line) {
    if (line.empty()) {
        return true;
    }
    std::string method = "";
    std::string url = "";
    std::string version = "";

    std::string::size_type first_blank = line.find(" ");
    if (first_blank == 0 || first_blank == line.length() - 1 || 
            first_blank == std::string::npos) {
        // 空格在开头、尾部、没找到这三种情况都是非法格式
        LOG(WARNING) << "[HTTPConnection::_parse_request_line]: first blank illegal";
        return false;
    }

    std::string::size_type second_blank = line.find(" ", first_blank + 1);
    if (second_blank == 0 || second_blank == line.length() - 1 || 
            second_blank == std::string::npos || second_blank <= first_blank) {
        LOG(WARNING) << "[HTTPConnection::_parse_request_line]: second blank illegal";
        return false;
    }

    method = line.substr(0, first_blank);
    if (method.empty()) {
        LOG(WARNING) << "[HTTPConnection::_parse_request_line]: method illegal";
        return false;
    }

    url = line.substr(first_blank + 1, second_blank - first_blank - 1);
    if (url.empty()) {
        LOG(WARNING) << "[HTTPConnection::_parse_request_line]: url illegal";
        return false;
    }

    version = line.substr(second_blank + 1);
    if (version.empty()) {
        LOG(WARNING) << "[HTTPConnection::_parse_request_line]: version illegal";
        return false;
    }

    _request.set_method(method);
    _request.set_url(url);
    _request.set_version(version);
    return true;
}

bool HTTPConnection::_parse_header(const std::string& line) {
    if (line.empty()) {
        return true;
    }

    std::string::size_type start = 0;
    std::string::size_type tail = line.find(":");
    if (tail == std::string::npos) {
        LOG(WARNING) << "[HTTPConnection::_parse_header]: cannot find colon";
        return false;
    }
    while (line[start++] == ' ');
    while (line[--tail] == ' ');

    std::string key = line.substr(start - 1, tail - start + 2);
    if (key.empty()) {
        LOG(WARNING) << "[HTTPConnection::_parse_header]: key is empty";
        return false;
    }

    start = line.find(":") + 1;
    tail = line.length();
    while (line[start++] == ' ');
    while (line[--tail] == ' ');
    std::string val = line.substr(start - 1, tail - start + 2);
    if (val.empty()) {
        LOG(WARNING) << "[HTTPConnection::_parse_header]: value is empty";
        return false;
    }

    _request.add_header(key, val);
    return true;
}

bool HTTPConnection::_parse_body(Buffer& buffer) {
    std::string content = "";
    char* body = nullptr;

    if (!buffer.read_until_end(&content)) {
        LOG(WARNING) << "[HTTPConnection::_parse_body]: read_until_end failed";
        return false;
    }
    if (content.empty()) {
        return true;
    }
    
    body = new (std::nothrow) char[content.length()];
    if (body == nullptr) {
        LOG(WARNING) << "[HTTPConnection::_parse_body]: new body failed";
        return false;
    }

    _request.set_body(body);
    _request.set_body_len(content.length());
    return true;
}

HTTPCode HTTPConnection::_handle_request() {
    if (_request.get_method() == "GET") {
        return _handle_get();
    }
}

// 根据_request来生成响应报文
HTTPCode HTTPConnection::_handle_get() {
    // 1、响应请求体
    std::string file_path = _get_root_dir() + _request.get_url();
    if (file_path.back() == '/') {
        file_path += "homepage.html";
    }

    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) < 0) {
        return HTTPCode::HTTP_NOT_FOUND;
    }
    if (!(file_stat.st_mode & S_IROTH)) {
        return HTTPCode::HTTP_FORBIDDEN;
    }
    if (S_ISDIR(file_stat.st_mode)) {
        return HTTPCode::HTTP_BAD_REQUEST;
    }

    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd == -1) {
        LOG(WARNING) << "[HTTPConnection::_handle_get]: open file failed";
        return HTTPCode::HTTP_INTERNAL_ERROR;
    }

    void* body = mmap(nullptr, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (body == MAP_FAILED) {
        LOG(WARNING) << "[HTTPCOnnection::_handle_get]: map failed";
        return HTTPCode::HTTP_INTERNAL_ERROR;
    }

    _response.set_body(reinterpret_cast<char*>(body));
    _response.set_body_len(file_stat.st_size);

    // 2、响应报头
    std::string ext_name = file_path.substr(file_path.rfind(".") + 1);
    if (_mime_type.find(ext_name) == _mime_type.end()) {
        _init_mime();
    }
    if (_mime_type.find(ext_name) == _mime_type.end()) {
        LOG(WARNING) << "[HTTPConnection::_handle_get]: mime type illegal, "
                << "extend name:" << ext_name << " file path:" << file_path << "*";
        return HTTPCode::HTTP_INTERNAL_ERROR;
    }
    _response.add_header("Content-Type", _mime_type[ext_name]);
    _response.add_header("Content-Length", std::to_string(file_stat.st_size));

    // 3、响应行
    _response.set_version(_request.get_version());
    _response.set_status("200");
    _response.set_info("OK");

    close(fd);
    return HTTPCode::HTTP_OK;
}


}
