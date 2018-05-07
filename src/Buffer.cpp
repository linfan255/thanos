//
// Created by van on 17-9-4.
//

#include <cstring>
#include <iostream>
#include "../inc/Buffer.h"

std::string Buffer::getline() {
    std::string ret = "";

    bool newlineReached = false;
    size_t len = buf.size();

    for(size_t i = read_pos; i < len; ++i) {
        if(buf[i] == 13 || buf[i] == 10) {
            newlineReached = true;
            read_pos = i;
            break;
        }
        ret += buf[i];
    }

    if(!newlineReached) {
        ret = "";
        return ret;
    }

    for(size_t i = read_pos; i < len; ++i) {
        if(buf[i] != 13 && buf[i] != 10) {
            read_pos = i;
            break;
        }
    }

    return ret;
}

void Buffer::putbytes(const char *src, ssize_t len) {
    buf.insert(buf.end(), src, src + len);
}

/*一直读到目标字符为止，以string的形式返回*/
std::string Buffer::getUntil(char delim) {
    size_t len = buf.size();
    std::string ret = "";

    for(size_t i = read_pos; i < len; i++) {
        if(buf[i] == delim) {
            read_pos = i + 1;
            break;
        }
        ret += buf[i];
    }

    return ret;
}

