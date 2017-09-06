//
// Created by van on 17-9-4.
//

#ifndef TINY_SERVER_BUFFER_H
#define TINY_SERVER_BUFFER_H


#include <vector>
#include <string>

class Buffer {
private:
    std::vector<char> buf;
    size_t read_pos;

public:
    Buffer(): read_pos(0) {};
    virtual ~Buffer() = default;

    std::string getline();
    void putbytes(const char* src, ssize_t len);
    std::string getUntil(char delim);

    const void* get_read_addr() const { return &buf[read_pos]; }
    const void* get_buffer_addr() const {
        if(buf.empty()) return nullptr;
        return &buf[0];
    }
    size_t get_read_pos() const { return read_pos; }
    size_t size() const { return buf.size(); }

    void clear() { buf.clear(); read_pos = 0; }
};


#endif //TINY_SERVER_BUFFER_H
