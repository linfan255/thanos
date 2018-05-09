#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <sys/epoll.h>
#include "threadpool.h"
#include "client.h"

using namespace std;

extern void add_fd(int epollfd, int fd, bool one_shot);
extern void remove_fd(int epollfd, int fd);


const int MAX_CLIENT = 65535;
const int MAX_EVENT = 10000;

int main() {
    Threadpool<Client> pool;

    Client* usrs = new Client[MAX_CLIENT];
    assert(usrs);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    if(ret < 0) {
        perror("bind");
        return 1;
    }

    ret = listen(listenfd, 5);
    assert(ret >= 0);

    epoll_event events[10000];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);

    add_fd(epollfd, listenfd, false);
    Client::epollfd = epollfd;

    for(;;) {
        int num = epoll_wait(epollfd, events, MAX_EVENT, -1);
        if(num < 0 && errno != EINTR) {
            cout << "epoll wait" << endl;
            break;
        }

        for(int i = 0; i < num; ++i) {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd) {
                struct sockaddr_in client_addr;
                socklen_t client_addlen = sizeof(client_addr);
                int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addlen);
                if(connfd < 0) {
                    cerr << "accept error" << endl;
                    continue;
                }

                usrs[connfd].init(connfd, client_addr);
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                usrs[sockfd].client_close();
            else if(events[i].events & EPOLLIN) {
                if(usrs[sockfd].client_read())
                    pool.add_task(usrs + sockfd);
                else
                    usrs[sockfd].client_close();
            }
            else if(events[i].events & EPOLLOUT) {
                if(!usrs[sockfd].client_write())
                    usrs[sockfd].client_close();
            }
        }
    }

    close(epollfd);
    close(listenfd);
    delete[] usrs;
    return 0;
}
