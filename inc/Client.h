//
// Created by van on 17-9-5.
//

#ifndef TINY_SERVER_CLIENT_H
#define TINY_SERVER_CLIENT_H


#include <netinet/in.h>
#include "HTTPResponse.h"

const int BUFFER_SIZE = 2048;

class Client
{
public:
    static int usr_cnt;
    static int epollfd;

private:
    int client_fd;
    sockaddr_in client_addr;
    HTTPResponse* resp;
    HTTPRequest* req;

    char client_buffer[BUFFER_SIZE];
    size_t curr_pos, read_pos;  //用来记录当前读写的位置

public:
    Client(): client_fd(-1), client_addr(), resp(nullptr), req(nullptr) {}
    ~Client() {
        delete resp;
        delete req;
    }

    bool client_read();
    bool client_write();
    void process();
    void client_close();
    void init(int fd, sockaddr_in addr);

private:
    void client_reset();
};


#endif //TINY_SERVER_CLIENT_H
