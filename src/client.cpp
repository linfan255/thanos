//
// Created by van on 17-9-5.
//

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "client.h"

int Client::epollfd = -1;
int Client::usr_cnt = 0;

int set_nonblock(int fd) {
    int old_opt = fcntl(fd, F_GETFL);
    int new_opt = old_opt | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_opt);
    return old_opt;
}

void add_fd(int epollfd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if(one_shot)    event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

    set_nonblock(fd);
}

void remove_fd(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void mod_fd(int epollfd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

bool Client::client_read() {
    if(read_pos >= BUFFER_SIZE)
        return false;

    int read_bytes = 0;
    for(;;) {
        read_bytes = recv(client_fd, client_buffer + read_pos, BUFFER_SIZE-read_pos, 0);

        if(read_bytes == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) break;
            return false;
        }
        else if(read_bytes == 0) {
            return false;
        }
        read_pos += read_bytes;
    }
    return true;
}

void Client::init(int fd, sockaddr_in addr) {
    client_fd = fd;
    client_addr = addr;
    resp = new HTTPResponse();
    req = new HTTPRequest();

    memset(client_buffer, 0, sizeof(client_buffer));
    curr_pos = read_pos = 0;

    add_fd(epollfd, fd, true);
    usr_cnt++;
}

bool Client::client_write() {
    ssize_t sent_bytes;
    int bytes_to_send = resp->size();
    int bytes_have_sent = 0;

    if(bytes_to_send == 0) {
        mod_fd(epollfd, client_fd, EPOLLIN);
        client_reset();
        return true;
    }

    for(;;) {
        sent_bytes = send(client_fd, resp->get_buffer_addr(), bytes_to_send, 0);
        if(sent_bytes <= -1) {
            if(errno == EAGAIN) {
                mod_fd(epollfd, client_fd, EPOLLIN);
                return true;
            }
            return false;
        }

        bytes_to_send -= sent_bytes;
        bytes_have_sent += sent_bytes;
        if(bytes_to_send <= bytes_have_sent) {
            if(resp->get_connection()) {
                client_reset();
                mod_fd(epollfd, client_fd, EPOLLIN);
                return true;
            } else {
                mod_fd(epollfd, client_fd, EPOLLIN);
                return false;
            }
        }
    }
}

void Client::client_reset() {
    resp->response_reset();
    req->request_reset();
    memset(client_buffer, 0, sizeof(client_buffer));
    curr_pos = read_pos = 0;
}

void Client::process() {
    req->create(client_buffer, read_pos);
    req->parse();
    resp->parse(*req);
    resp->create();

    //resp->echo_response();

    mod_fd(epollfd, client_fd, EPOLLOUT);
}

void Client::client_close() {
    if(client_fd != -1) {
        remove_fd(epollfd, client_fd);
        client_fd = -1;
        usr_cnt--;
    }
}
