#ifndef _THANOS_FDHANDLER
#define _THANOS_FDHANDLER

namespace thanos {

class FdHandler {
public:
    FdHandler() = default;
    ~FdHandler() = default;

    static int set_nonblock(int fd);
    static bool add_fd(int epollfd, int fd, bool one_shot);
    static bool remove_fd(int epollfd, int fd);
    static bool mod_fd(int epollfd, int fd, int ev);
};

int FdHandler::set_nonblock(int fd) {
    int old_opt = fcntl(fd, F_GETFL);
    if (old_opt == -1) {
        LOG(WARNING) << "[Server::_set_nonblock]: get fcntl failed";
        return -1;
    }
    int new_opt = old_opt | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, new_opt) == -1) {
        LOG(WARNING) << "[Server::_set_nonblock]: set fcntl failed";
        return -1;
    }
    return old_opt;
}

bool FdHandler::add_fd(int epollfd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if (one_shot) {
        event.events |= EPOLLONESHOT;
    }
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1) {
        LOG(WARNING) << "[Server::_add_fd]: epoll_ctl failed";
        return false;
    }
    if (set_nonblock(fd) == -1) {
        LOG(WARNING) << "[Server::_add_fd]: _set_nonblock failed";
        return false;
    }
    return true;
}

bool FdHandler::remove_fd(int epollfd, int fd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0) == -1) {
        LOG(WARNING) << "[Server::_remove_fd]: _remove_fd failed";
        return false;
    }
    if (close(fd) == -1) {
        LOG(WARNING) << "[Server::_remove_fd]: close fd failed";
        return false;
    }
    return true;
}

bool FdHandler::mod_fd(int epollfd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) == -1) {
        LOG(WARNING) << "[Server::_mod_fd]: epoll_ctl failed";
        return false;
    }
    return true;
}

}

#endif
