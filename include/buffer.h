#ifndef _THANOS_BUFFER
#define _THANOS_BUFFER

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

    void push_nbytes(char* buffer, uint64_t nbytes);
    void roll_nbytes(uint64_t nbytes);

    BufferReadStatus get_line(std::string* line);
    void* current_addr() const;

private:
    uint64_t _cursor; // 记录当前读的位置
    std::vector<char> _buffer;
};

}

#endif
