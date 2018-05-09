//
// Created by van on 17-9-5.
//

#ifndef TINY_SERVER_HTTPRESPONSE_H
#define TINY_SERVER_HTTPRESPONSE_H


#include "http_message.h"
#include <iostream>
#include "http_request.h"

const int STATUS_NUM = 4;
const std::string STATUS_CODE[STATUS_NUM] = {"200", "403", "404", "400"};
const std::string STATUS_STR[STATUS_NUM] = {"OK", "Forbidden", "Not Found", "Bad Request"};
const std::string ERROR_CAUSE[STATUS_NUM] = {
        "success", "users do not have permission!!",
        "required file does not exist!!", "illegal request!!"
};
const std::string BASE_DIR = "/home/van/CLionProjects/tiny_server/doc";

class HTTPResponse: public HTTPMessage {
private:
    std::string version;
    std::string status_code;
    std::string status;
    std::string mime_type;  //请求资源的mime类型
    bool connection;

private:
    void parse_uri(const std::string& uri);
    void parse_mimeType(const std::string& uri);

    std::string code2str(const std::string& code);
    std::string get_extname(const std::string& uri);
    std::string get_error_cause(const std::string& code);
    void client_error();

public:
    HTTPResponse(): HTTPMessage() {};
    virtual ~HTTPResponse() = default;

    void parse(const HTTPRequest& req);
    void create();

    bool get_connection() const { return connection; }

    void response_reset() {
        message_reset();
        version.clear();
        status_code.clear();
        status.clear();
        mime_type.clear();
        connection = false;
    }

};


#endif //TINY_SERVER_HTTPRESPONSE_H
