//
// Created by van on 17-9-5.
//

#include "../inc/HTTPRequest.h"

void HTTPRequest::create(char *src, size_t len) {
    putbytes(src, len);
}

void HTTPRequest::parse() {
    method = getUntil(' ');
    uri = getUntil(' ');
    version = getline();

    parse_header();

    auto it = headers.find("Connection");
    if(it == headers.end()) {
        //默认是短连接
        connection = false;
    }
    else {
        connection = (it->second == "keep-alive" ? true : false);
    }

    parse_body();
}
