#include <iostream>
#include <cstdio>
#include <cstring>
#include <new>
#include <string>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
    int sockfd;
    sockaddr_in serveraddr;

    if (argc != 2) {
        std::cerr << "usage: ./client_test <server ip>" << std::endl;
        return 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        LOG(WARNING) << "socket fialed";
        return 0;
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8080);

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == -1) {
        LOG(WARNING) << "connect failed";
        return 0;
    }

    char sendline[1024], recvline[1024];
    while (fgets(sendline, 1024, stdin) != NULL) {
        send(sockfd, sendline, strlen(sendline), 0);
        recv(sockfd, recvline, 1024, 0);
        printf("echo:%s", recvline);
    }
    return 0;
}
