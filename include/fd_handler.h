#ifndef _THANOS_FDHANDLER_H
#define _THANOS_FDHANDLER_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "easylogging++.h"

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


}

#endif
