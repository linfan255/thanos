#ifndef _THANOS_BUFFER_H
#define _THANOS_BUFFER_H

#include <vector>
#include <string>
#include <stdint.h>
#include "common_type.h"

namespace thanos {

// 写缓存相关成员：push_nbytes
// 读缓存相关成员：get_line 
//   current_addr roll_nbytes _cursor等成员都是用来配合读操作
class Buffer {
public:
    Buffer();
    ~Buffer();

    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);

    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);

    uint64_t size() const;
    bool empty() const;
    void clear();
    bool dump(Buffer* other);

    void push_nbytes(const char* buffer, uint64_t nbytes);
    void roll_nbytes(uint64_t nbytes);

    BufferReadStatus get_line(std::string* line);
    bool read_until_end(std::string* content);
    void* current_addr() const;
    uint64_t left_bytes() const;

    void show_content() const;

private:
    uint64_t _cursor; // 记录当前读的位置
    std::vector<char> _buffer;
};

}

#endif
