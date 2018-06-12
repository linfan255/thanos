#ifndef _THANOS_COMMON_TYPE_H
#define _THANOS_COMMON_TYPE_H

namespace thanos {

// this enum class is maily used by Buffer
enum class BufferReadStatus {
    READ_SUCCESS = 0, // success
    READ_END = 1, // reach buffer's end
    READ_FAIL = -1, // can not find a new line. (can't find \n\r in buffer)
    READ_ERROR = -2, // parameter is nullptr
    READ_OUTBOUND = -3, // _cursor of Buffer out of bound
};

enum class ParseStatus {
    PARSE_DONE = 0,         // parse done
    PARSE_HEADER = 1,       // parsing headers
    PARSE_REQUEST_LINE = 2, // parsing request line
    PARSE_BODY = 3,         // parsing body
    PARSE_ERROR = -1,       // parsing error
};

enum class HTTPCode {
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_INTERNAL_ERROR = 500,
};

}

#endif
