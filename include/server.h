// Created by linfan255 on 2018-06-07
// 服务器的总入口
// 使用方法：
//   1、init传入配置文件的路径
//   2、启动run
//   3、调用uninit在退出后清理有关资源
#ifndef _THANOS_SERVER_H
#define _THANOS_SERVER_H

#include <map>
#include <string>
#include <sys/epoll.h>
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

    bool _handle_event(const epoll_event& ev);
    bool _handle_accept();
    bool _handle_readable(const epoll_event& ev);
    bool _handle_writable(const epoll_event& ev);
    //////////////////////////////////////////////////

    Configure _conf; // 配置类工具库
    int _listenfd; // 负责监听的描述符
    int _epollfd; // epoll描述符
    std::map<int, Connection*> _connections; // 记录每个描述符所对应的连接
    ThreadPool<Connection> _threadpool; // 线程池
    bool _is_running; // 服务是否在跑

    // configure value
    int _port;
    int _backlog;
    int _max_events;
    int _max_thread_num;
    int _max_requests;
    std::string _root_dir; // 客户端请求的根目录

    // disallow copy & assign
    Server(const Server& other);
    Server& operator=(const Server& other);
};

}

#endif
