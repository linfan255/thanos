#include <cstring>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"
#include "fd_handler.h"
#include "easylogging++.h"

namespace thanos {

Connection::Connection() : _read_buffer(), _write_buffer(), _connfd(-1) {}

Connection::~Connection() = default;

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

    uint64_t buffer_size = 2048;
    int ret = 0;
    char buffer[buffer_size];

    while (true) {
        memset(buffer, 0, sizeof(char) * buffer_size);
        ret = recv(_connfd, buffer, buffer_size, 0);

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
        }

        if (ret == -1 || ret == 0) {
            LOG(WARNING) << "[Connection::connection_read]: "
                    << "recv failed or client close connection";
            return false;
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
        return true;
    }

    int64_t ret = 0;
    while (true) {
        if (_write_buffer.current_addr() == nullptr) {
            LOG(WARNING) << "[Connection::connection_write]: get current_addr failed";
            return false;
        }

        ret = send(_connfd, _write_buffer.current_addr(), _write_buffer.size(), 0);

        if (ret == -1 && errno != EINTR) {
            LOG(WARNING) << "[Connection::connection_write]: send failed";
            return false;
        }
        if (ret == _write_buffer.size()) {
            break;
        }
        if (ret == 0) { // 发送0字节，可能没发送连接就断开了
            LOG(WARNING) << "[Connection::connection_write]: send 0 bytes";
            return false;
        }
        _write_buffer.roll_nbytes(ret);
    }
    return true;
}

} // namespace thanos
