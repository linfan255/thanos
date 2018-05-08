//
// Created by van on 17-9-5.
//

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include "../include/HTTPResponse.h"

void HTTPResponse::parse(const HTTPRequest &req) {
    version = req.get_version();
    connection = req.get_conn();
    parse_uri(req.get_uri());
}

void HTTPResponse::parse_uri(const std::string& uri) {
    std::string file_dir = BASE_DIR + uri;

    if(file_dir.back() == '/')
        file_dir += "homepage.html";

    parse_mimeType(file_dir);

    struct stat file_stat;

    if(stat(file_dir.c_str(), &file_stat) < 0) {
        status_code = "404";
        status = code2str(status_code);
        return;
    }

    if(!(file_stat.st_mode & S_IROTH)) {
        status_code = "403";
        status = code2str(status_code);
        return;
    }

    if(S_ISDIR(file_stat.st_mode)) {
        status_code = "400";
        status = code2str(status_code);
        return;
    }

    int fd = open(file_dir.c_str(), O_RDONLY);
    body = (char*)mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    bodylen = file_stat.st_size;
    close(fd);
    status_code = "200";
    status = code2str(status_code);
}

std::string HTTPResponse::code2str(const std::string &code) {
    int i;
    for(i = 0; i < STATUS_NUM; i++) {
        if(STATUS_CODE[i] == code)
            break;
    }
    return STATUS_STR[i];
}

void HTTPResponse::create() {
    std::string status_line = version + " " + status_code + " " + status + "\r\n";

    putbytes(status_line.c_str(), status_line.length());

    if(status != "200")
        mime_type = "text/html";
    add_header("Content-Type", mime_type);

    char body_len[20];
    sprintf(body_len, "%d", get_bodylen());
    std::string len(body_len);
    add_header("Content-Length", len);

    if(connection)  add_header("Connection", "keep-alive");
    else            add_header("Connection", "close");

    create_header();

    if(status_code != "200")
        client_error();

    if(body && bodylen != 0) {
        create_body();

        if(status_code == "200")
            munmap(body, bodylen);
        else
            delete[] body;

        body = nullptr;
    }
}

void HTTPResponse::parse_mimeType(const std::string &uri) {
    std::string ext_name = get_extname(uri);
    std::ifstream file("/home/van/CLionProjects/lfServer/mime.txt");

    std::stringstream ss;
    std::string line, key;
    mime_type = "";

    while(std::getline(file, line)) {
        ss.str(line);
        ss >> mime_type >> key;
        ss.clear();

        if(key == ext_name)
            break;
    }
}

std::string HTTPResponse::get_extname(const std::string &uri) {
    size_t pos = uri.find('.');
    return uri.substr(pos + 1);
}

void HTTPResponse::client_error() {
    char buf[2048];
    sprintf(buf, "<html><title>lfServer Error</title>");
    sprintf(buf, "%s<body bgcolor=""ffffff"">\r\n", buf);
    sprintf(buf, "%s%s: %s\r\n", buf, status_code.c_str(), status.c_str());
    sprintf(buf, "%s<p>%s\r\n", buf, get_error_cause(status_code).c_str());
    sprintf(buf, "%s<hr><em>The lf Web server</em>\r\n", buf);

    bodylen = strlen(buf);
    body = new char[bodylen];
    memcpy(body, buf, bodylen);
}

std::string HTTPResponse::get_error_cause(const std::string &code) {
    int i;
    for(i = 0; i < STATUS_NUM; i++)
        if(STATUS_CODE[i] == code)
            break;
    return ERROR_CAUSE[i];
}
