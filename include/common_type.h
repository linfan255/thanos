#ifndef _THANOS_COMMON_TYPE
#define _THANOS_COMMON_TYPE

namespace thanos {

enum class BufferReadStatus {
    READ_SUCCESS = 0, // success
    READ_END = 1, // reach buffer's end
    READ_FAIL = -1, // can not find a new line. (can't find \n\r in buffer)
    READ_ERROR = -2, // parameter is nullptr
    READ_OUTBOUND = -3, // _read_pos of Buffer out of bound
};

}

#endif
