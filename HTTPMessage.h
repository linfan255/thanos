//
// Created by van on 17-9-4.
//

#ifndef TINY_SERVER_HTTPMESSAGE_H
#define TINY_SERVER_HTTPMESSAGE_H


#include <map>
#include <sys/mman.h>
#include "Buffer.h"

class HTTPMessage: public Buffer {
protected:
    std::map<std::string, std::string> headers;
    char* body;
    size_t bodylen;

public:
    HTTPMessage(): headers(), body(nullptr), bodylen(0) {}
    virtual ~HTTPMessage() = default;

    void create_header();
    void create_body();

    void parse_header();
    void parse_body();

    void add_header(const std::string& key, const std::string& value);

    void set_body(char* src, size_t len) {body = src; bodylen = len;}
    char* get_body() const { return body; }
    size_t get_bodylen() const { return bodylen; }

    void message_reset() {
        clear();
        headers.clear();
        if(body) {
            munmap(body, bodylen);
            body = nullptr;
            bodylen = 0;
        }
    }
};


#endif //TINY_SERVER_HTTPMESSAGE_H
