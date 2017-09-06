//
// Created by van on 17-9-4.
//

#include "HTTPMessage.h"

/*根据headers成员中的值往缓冲区中填写信息*/
void HTTPMessage::create_header() {
    std::string key, value, line;
    for(auto it = headers.begin(); it != headers.end(); ++it) {
        key = it->first;
        value = it->second;
        line = key + ": " + value + "\r\n";
        putbytes(line.c_str(), line.length());
    }
    putbytes("\r\n", 2);
}

/*
 * 向缓冲区中写入实体
 */
void HTTPMessage::create_body() {
    putbytes(body, bodylen);
}

/*根据缓冲区中的信息解析出首部*/
void HTTPMessage::parse_header() {
    std::string key, value, line;
    while(!(line = getline()).empty()) {
        size_t pos = line.find(':');
        key = line.substr(0, pos);
        value = line.substr(pos + 1);

        pos = 0;
        while(value[pos] == ' ') pos++;
        value = value.substr(pos);

        headers.insert(std::make_pair(key, value));
    }
}

//根据缓冲区内容解析出实体
void HTTPMessage::parse_body() {
    body = (char*)get_read_addr();
    bodylen = size() - get_read_pos();
}

void HTTPMessage::add_header(const std::string &key, const std::string &value) {
    headers.insert(std::make_pair(key, value));
}
