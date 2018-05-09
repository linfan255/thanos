//
// Created by van on 17-9-5.
//

#ifndef TINY_SERVER_HTTPREQUEST_H
#define TINY_SERVER_HTTPREQUEST_H

#include <string>
#include "http_message.h"


class HTTPRequest: public HTTPMessage {
private:
    std::string version;
    std::string method;
    std::string uri;
    bool connection;    //长连接为true，短连接为false

public:
    HTTPRequest(): HTTPMessage() {};
    virtual ~HTTPRequest() = default;

    void create(char* src, size_t len);
    void parse();

    std::string get_version() const { return version; }
    std::string get_method() const { return method; }
    std::string get_uri() const { return uri; }
    bool get_conn() const { return connection; }

    void request_reset() {
        message_reset();
        version.clear();
        method.clear();
        uri.clear();
        connection = false;
    }
};


#endif //TINY_SERVER_HTTPREQUEST_H
