#ifndef _THANOS_HTTPMESSAGE_H 
#define _THANOS_HTTPMESSAGE_H 

#include <string>
#include <map>
#include <stdint.h>
#include "common_type.h"

namespace thanos {

class HTTPMessage {
public:
    HTTPMessage();
    virtual ~HTTPMessage();

    HTTPMessage(const HTTPMessage& other);
    HTTPMessage& operator=(const HTTPMessage& other);

    HTTPMessage(const HTTPMessage&& other);
    HTTPMessage& operator=(const HTTPMessage&& other);

    // get
    std::map<std::string, std::string>& get_headers() const;
    char* get_body() const;
    uint64_t get_body_len() const;

    // set
    void add_header(const std::string& key, const std::string& val);
    bool set_body(char* body);
    void set_body_len();

private:
    std::map<std::string, std::string> _headers;
    char* _body;
    uint64_t _body_len;
};

/////////////////////////////////////////////////////////////////////////////

class HTTPRequest : public HTTPMessage {
public:
    HTTPRequest();
    ~HTTPRequest();

    // copy ctor & assign
    HTTPRequest(const HTTPRequest& other);
    HTTPRequest& operator=(const HTTPRequest& other);

    // move ctor & assign
    HTTPRequest(const HTTPRequest&& other);
    HTTPRequest& operator=(const HTTPRequest& other);

    // get
    std::string& get_method() const;
    std::string& get_url() const;
    std::string& get_version() const;

    // set
    void set_method(const std::string& method);
    void set_url(const std::string& url);
    void set_version(const std::string& version);

private:
    std::string _method;
    std::string _url;
    std::string _version;
};

////////////////////////////////////////////////////////
class HTTPResponse : public HTTPMessage {
public:
    HTTPResponse();
    ~HTTPResponse();

    // copy ctor & assign
    HTTPResponse(const HTTPResponse& other);
    HTTPResponse& operator=(const HTTPResponse& other);

    // move ctor & assign
    HTTPResponse(const HTTPResponse&& other);
    HTTPResponse& operator=(const HTTPResponse&& other);

    // get
    std::string& get_version() const;
    std::string& get_status() const;
    std::string& get_info() const;
    // set
    void set_version(const std::string& version);
    void set_status(const std::string& status);
    void set_info(const std::string& info);

private:
    std::string _version;
    std::string _status;
    std::string _info;
};

}

#endif
