#ifndef _THANOS_HTTP_CONNECTION_H
#define _THANOS_HTTP_CONNECTION_H

#include <map>
#include "connection.h"
#include "http_message.h"
#include "common_type.h"

namespace thanos {

class HTTPConnection : public Connection {
public:
    HTTPConnection(int dummy); // this ctor only used by _clone
    ~HTTPConnection(); 

    void process() override;

protected:
    HTTPConnection();          // add prototype in this ctor

    Connection* _clone() override;
    bool _clear() override;
    bool _is_keep_alive() override;

    static HTTPConnection _http_connection;

private:
    void _parse_request(Buffer& buffer);
    bool _parse_request_line(const std::string& line);
    bool _parse_header(const std::string& line);
    bool _parse_body(Buffer& buffer);

    bool _create_response_stream(Buffer* buffer);

    std::string _make_status(const HTTPCode& http_code);
    std::string _make_status_info(const HTTPCode& http_code);
    void _error_response(const HTTPCode& http_code);

    HTTPCode _handle_request();
    HTTPCode _handle_get();

    static void _init_mime();

    static std::map<std::string, std::string> _mime_type;
    HTTPRequest _request;
    HTTPResponse _response;
    ParseStatus _parse_status;
    bool _connection;

    // disallow copy & assign
    HTTPConnection(const HTTPConnection& other);
    HTTPConnection& operator=(const HTTPConnection& other);
};

}

#endif
