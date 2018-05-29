#ifndef _THANOS_BUFFER
#define _THANOS_BUFFER

#include <vector>
#include <string>
#include <stdint.h>
#include "common_type.h"

namespace thanos {

class Buffer {
public:
    Buffer();
    ~Buffer();

    uint64_t size() const;
    bool empty() const;
    void clear();

    void push_nbytes(char* buffer, uint64_t nbytes);
    void roll_nbytes(uint64_t nbytes);

    BufferReadStatus get_line(std::string* line);
    void* get_write_addr() const;

private:
    uint64_t _write_pos;
    uint64_t _read_pos;
    std::vector<char> _buffer;
};

}

#endif
