#ifndef _THANOS_CONNECTION
#define _THANOS_CONNECTION

#include "buffer.h"

namespace thanos {

class Connection {
public:
    Connection();
    virtual ~Connection();

    static Connection* new_instance();
    static int get_epollfd();
    static void set_epollfd(int epollfd);

    bool connection_close();
    bool connection_read();
    bool connection_write();

    // 用于处理业务逻辑，将由子类实现，
    // 并在线程池中用于线程的执行函数
    virtual bool process() = 0; 

    void set_connfd(int connfd);
    int get_connfd();

protected:
    static void _add_prototype(Connection* conn);
    virtual Connection* _clone() = 0; // prototype模式所需，用于返回实例化的对象指针
    virtual bool _clear() = 0; // 用于告诉框架子类在断开连接后需要进行哪些处理

private:
    static Connection* _prototype;
    static int _epollfd;

    Buffer _read_buffer;
    Buffer _write_buffer;
    int _connfd;
};

Connection* Connection::_prototype;
int Connection::_epollfd = -1;

void Connection::_add_prototype(Connection* conn) {
    _prototype = conn;
}

Connection* Connection::new_instance() {
    return _prototype->clone();
}

void Connection::set_epollfd(int epollfd) {
    _epollfd = epollfd;
}

int Connection::get_epollfd() {
    return _epollfd;
}

} // namespace thanos

#endif
