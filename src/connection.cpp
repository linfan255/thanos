#include <cstring>
#include <cstdio>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"
#include "fd_handler.h"
#include "easylogging++.h"

namespace thanos {

Connection* Connection::_prototype = nullptr;

int Connection::_epollfd = -1;

std::string Connection::_root_dir = "";

std::string& Connection::_get_root_dir() {
    return _root_dir; 
}

void Connection::init_root_dir(const std::string& root_dir) {
    _root_dir = root_dir;
}

void Connection::_add_prototype(Connection* conn) {
    _prototype = conn;
}

Connection* Connection::new_instance() {
    return _prototype->_clone();
}

void Connection::set_epollfd(int epollfd) {
    _epollfd = epollfd;
}

int Connection::get_epollfd() {
    return _epollfd;
}

Connection::Connection() : _read_buffer(), _write_buffer(), _connfd(-1) {}

Connection::~Connection() = default;

void Connection::_reset() {
    _read_buffer.clear();
    _write_buffer.clear();
    if (!_clear()) {
        LOG(WARNING) << "[Connection::_reset]: _clear";
    }
}

void Connection::set_connfd(int connfd) {
    _connfd = connfd;
}

int Connection::get_connfd() {
    return _connfd;
}

bool Connection::connection_close() {
    _read_buffer.clear();
    _write_buffer.clear();

    if (!FdHandler::remove_fd(_epollfd, _connfd)) {
        LOG(WARNING) << "[Connection::connection_close]: remove failed";
        return false;
    }
    _connfd = -1;
    
    if (!_clear()) {
        LOG(WARNING) << "[Connection::connection_close]: _clear() failed";
        return false;
    }
    LOG(INFO) << "[Connection::connection_close]: close connection success";
    return true;
}

// @breif: 主要功能是从套接字中读取数据到相关的读缓存当中，
// 为下一步的业务逻辑处理做好准备。
// @return: 处理成功返回true，否则false
bool Connection::connection_read() {
    if (_connfd == -1) {
        LOG(WARNING) << "[Connection::connection_read]: invalid fd";
        return false;
    }
    if (!_read_buffer.empty()) {
        _read_buffer.clear();
    }

    uint64_t buffer_size = 2048;
    int ret = 0;
    char buffer[buffer_size];

    while (true) {
        memset(buffer, 0, sizeof(char) * buffer_size);
        ret = recv(_connfd, buffer, buffer_size, 0);

        if (ret == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            LOG(WARNING) << "[Connection::connection_read]: recv failed";
            return false;
        } else if (ret == 0) {
            //LOG(WARNING) << "[Connection::connection_read]: client close connection";
            return true;
        }

        _read_buffer.push_nbytes(buffer, ret * sizeof(char));
    }
    return true;
}

bool Connection::connection_write() {
    if (_write_buffer.empty()) {
        if (!FdHandler::mod_fd(_epollfd, _connfd, EPOLLIN)) {
            LOG(WARNING) << "[Connection::connection_write]: mod_fd failed";
            return false;
        }
        _reset();
        return true;
    }

    int64_t ret = 0;
    while (true) {
        if (_write_buffer.left_bytes() == 0) {
            if (!FdHandler::mod_fd(_epollfd, _connfd, EPOLLIN)) {
                LOG(WARNING) << "[Connection::connection_write]: mod_fd to EPOLLIN failed";
                return false;
            }
            _reset();
            return true;
        }

        if (_write_buffer.current_addr() == nullptr) {
            LOG(WARNING) << "[Connection::connection_write]: get current_addr failed";
            return false;
        }

        ret = send(_connfd, _write_buffer.current_addr(), _write_buffer.left_bytes(), 0);

        if (ret == 0) {
            if (!FdHandler::mod_fd(_epollfd, _connfd, EPOLLIN)) {
                LOG(WARNING) << "[Connection::connection_write]: mod_fd to EPOLLIN failed";
                return false;
            }
            _reset();
            return true;
        }

        if (ret <= -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if (!FdHandler::mod_fd(_epollfd, _connfd, EPOLLIN)) {
                    LOG(WARNING) << "[Connection::connection_write]: mod_fd to EPOLLIN failed";
                    return false;
                }
                _reset();
                return true;
            } else {
                //LOG(WARNING) << "[Connection::connection_write]: send failed: " << strerror(errno);
                //return false; 向已关闭的客户端写数据会产生Broken Pipe，但是不返回错误
                _reset();
                return true;
            }
        }
        _write_buffer.roll_nbytes(ret);
    }

    _reset();
    return true;
}

bool Connection::_dump_read(Buffer* other) {
    if (other == nullptr) {
        LOG(WARNING) << "[Connection::_dump_read]: dump read_buffer to no where";
        return false;
    }
    if (!other->empty()) {
        other->clear();
    }
    if (!_read_buffer.dump(other)) {
        LOG(WARNING) << "[Connection::_dump_read]: dump read_buffer failed";
        return false;
    }
    return true;
}

bool Connection::_dump_write(Buffer* other) {
    if (other == nullptr) {
        LOG(WARNING) << "[Connection::_dump_write]: dump write_buffer to no where";
        return false;
    }
    if (!other->empty()) {
        other->clear();
    }
    if (!_write_buffer.dump(other)) {
        LOG(WARNING) << "[Connection::_dump_write]: dump write_buffer failed";
        return false;
    }
    return true;
}

bool Connection::_dump_to_read(Buffer& other) {
    if (!_read_buffer.empty()) {
        _read_buffer.clear();
    }
    if (!other.dump(&_read_buffer)) {
        LOG(WARNING) << "[Connection::_dump_to_read]: dump to read failed";
        return false;
    }
    return true;
}

bool Connection::_dump_to_write(Buffer& other) {
    if (!_write_buffer.empty()) {
        _write_buffer.clear();
    }
    if (!other.dump(&_write_buffer)) {
        LOG(WARNING) << "[Connection::_dump_to_write]: dump to write failed";
        return false;
    }
    return true;
}

bool Connection::_process_done() {
    if (!FdHandler::mod_fd(_epollfd, _connfd, EPOLLOUT)) {
        LOG(WARNING) << "[Connection::_process_done]: mod_fd";
        return false;
    }
    return true;
}

} // namespace thanos
