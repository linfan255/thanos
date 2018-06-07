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
    virtual void process() = 0; 

    void set_connfd(int connfd);
    int get_connfd();

protected:
    static void _add_prototype(Connection* conn);
    virtual Connection* _clone() = 0; // prototype模式所需，用于返回实例化的对象指针
    virtual bool _clear() = 0; // 用于告诉框架子类在断开连接后需要进行哪些处理
    bool _process_done();      // 告诉框架业务逻辑处理完毕，可以写了

    bool _dump_read(Buffer* other);    // read/write buffer --> other
    bool _dump_write(Buffer* other);

    bool _dump_to_read(Buffer& other); // other --> read/write buffer
    bool _dump_to_write(Buffer& other);

private:
    static Connection* _prototype;
    static int _epollfd;

    Buffer _read_buffer;
    Buffer _write_buffer;
    int _connfd;
};


} // namespace thanos

#endif
