#include <utility>
#include <algorithm>
#include "http_message.h"
#include "easylogging++.h"

namespace thanos {

HTTPMessage::HTTPMessage() :
        _headers(), _body(nullptr), _body_len(0) {}

HTTPMessage::~HTTPMessage() {
    if (_body != nullptr) {
        delete _body;
    }
    _body_len = 0;
}

HTTPMessage::HTTPMessage(const HTTPMessage& other) :
        _headers(other._headers), _body(other._body), _body_len(other._body_len) {}

HTTPMessage& HTTPMessage::operator=(const HTTPMessage& other) {
    if (this != &other) {
        _headers = other._headers;
        _body = other._body;
        _body_len = other._body_len;
    }
    return *this;
}

HTTPMessage::HTTPMessage(HTTPMessage&& other) :
        _headers(std::move(other._headers)), 
        _body(other._body), 
        _body_len(other._body_len) {
    other._body = nullptr;
    other._body_len = 0;
}

HTTPMessage& HTTPMessage::operator=(HTTPMessage&& other) {
    if (this != &other) {
        _headers = std::move(other._headers);
        _body = other._body;
        _body_len = other._body_len;

        other._body = nullptr;
        other._body_len = 0;
    }
    return *this;
}

const std::map<std::string, std::string>& HTTPMessage::get_headers() const {
    return _headers;
}

char* HTTPMessage::get_body() const {
    return _body;
}

uint64_t HTTPMessage::get_body_len() const {
    return _body_len;
}

void HTTPMessage::add_header(const std::string& key, const std::string& val) {
    _headers.insert(std::make_pair(key, val));
}

bool HTTPMessage::set_body(char* body) {
    _body = body;
    return true;
}

void HTTPMessage::set_body_len(uint64_t body_len) {
    _body_len = body_len;
}

//////////////////////////////////////////////////////////////////////////

HTTPRequest::HTTPRequest() : HTTPMessage(), _method(""), _url(""), _version("") {}

HTTPRequest::~HTTPRequest() = default;

HTTPRequest::HTTPRequest(const HTTPRequest& other) :
        HTTPMessage(other), 
        _method(other._method), 
        _url(other._url), 
        _version(other._version) {}
        
HTTPRequest& HTTPRequest::operator=(const HTTPRequest& other) {
    if (this != &other) {
        HTTPMessage::operator=(other);
        _method = other._method;
        _url = other._url;
        _version = other._version;
    }
    return *this;
}

HTTPRequest::HTTPRequest(HTTPRequest&& other) :
        HTTPMessage(std::move(other)), 
        _method(std::move(other._method)), 
        _url(std::move(other._url)), 
        _version(std::move(other._version)) {}

HTTPRequest& HTTPRequest::operator=(HTTPRequest&& other) {
    if (this != &other) {
        HTTPMessage::operator=(std::move(other));
        _method = std::move(other._method);
        _url = std::move(other._url);
        _version = std::move(other._version);
    }
    return *this;
}

const std::string& HTTPRequest::get_method() const {
    return _method;
}

const std::string& HTTPRequest::get_url() const {
    return _url;
}

const std::string& HTTPRequest::get_version() const {
    return _version;
}

void HTTPRequest::set_method(const std::string& method) {
    _method = method;
}

void HTTPRequest::set_url(const std::string& url) {
    _url = url;
}

void HTTPRequest::set_version(const std::string& version) {
    _version = version;
}
//////////////////////////////////////////////////////////////

HTTPResponse::HTTPResponse() : HTTPMessage(), _version(""), _status(""), _info("") {}

HTTPResponse::~HTTPResponse() = default;

HTTPResponse::HTTPResponse(const HTTPResponse& other) :
        HTTPMessage(other),
        _version(other._version),
        _status(other._status),
        _info(other._info) {}

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& other) {
    if (this != &other) {
        HTTPMessage::operator=(other);
        _version = other._version;
        _status = other._status;
        _info = other._info;
    }
    return *this;
}

HTTPResponse::HTTPResponse(HTTPResponse&& other) :
        HTTPMessage(std::move(other)),
        _version(std::move(other._version)),
        _status(std::move(other._status)),
        _info(std::move(other._info)) {}

HTTPResponse& HTTPResponse::operator=(HTTPResponse&& other) {
    if (this != &other) {
        HTTPMessage::operator=(std::move(other));
        _version = std::move(other._version);
        _status = std::move(other._status);
        _info = std::move(other._info);
    }
    return *this;
}

const std::string& HTTPResponse::get_version() const {
    return _version;
}

const std::string& HTTPResponse::get_status() const {
    return _status;
}

const std::string& HTTPResponse::get_info() const {
    return _info;
}

void HTTPResponse::set_version(const std::string& version) {
    _version = version;
}

void HTTPResponse::set_status(const std::string& status) {
    _status = status;
}

void HTTPResponse::set_info(const std::string& info) {
    _info = info;
}

}
