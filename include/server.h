#ifndef _THANOS_SERVER
#define _THANOS_SERVER

#include <map>
#include <string>
#include "configure.h"
#include "threadpool.h"
#include "connection.h"

namespace thanos {

class Server {
public:
    Server();
    ~Server();

    bool init(const std::string& conf_path);
    bool run();
    bool uninit();

    void ask_to_quit();

private:
    bool _listen_at_port();
    bool _close_connection(int fd);

    int _set_nonblock(int fd);
    bool _add_fd(int fd, bool one_shot);
    bool _remove_fd(int fd);
    bool _mod_fd(int fd, int ev);

    bool _handle_event(const epoll_event& ev);
    bool _handle_accept();
    bool _handle_readable(const epoll_event& ev);
    bool _handle_writable(const epoll_event& ev);
    //////////////////////////////////////////////////

    Configure _conf; // 配置类工具库
    int _listenfd; // 负责监听的描述符
    int _epollfd; // epoll描述符
    std::map<int, Connection*> _connections; // 记录每个描述符所对应的连接
    ThreadPool _threadpool; // 线程池
    bool _is_running; // 服务是否在跑

    // configure value
    int _port;
    int _backlog;
    int _max_events;

    // disallow copy & assign
    Server(const Server& other);
    Server& operator=(const Server& other);
};

}

#endif
