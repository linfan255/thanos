#include "easylogging++.h"
#include "buffer.h"
#include "common_type.h"

namespace thanos {

Buffer::Buffer() : _buffer(), _cursor(0) {}

Buffer::~Buffer() = default;

uint64_t Buffer::size() const {
    return _buffer.size();
}

void Buffer::push_nbytes(char* buffer, uint64_t nbytes) {
    _buffer.insert(_buffer.end(), buffer, buffer + nbytes);
}

void Buffer::roll_nbytes(uint64_t nbytes) {
    _cursor += nbytes;
}

void* Buffer::current_addr() const {
    if (_buffer.empty() || _cursor >= _buffer.size()) {
        LOG(WARNING) << "[Buffer::current_addr]: _cursor wrong position";
        return nullptr;
    }
    char* addr = const_cast<char*>(&_buffer[_cursor]);
    return reinterpret_cast<void*>(addr);
}

void Buffer::clear() {
    _cursor = 0;
    _buffer.clear();
}

bool Buffer::empty() const {
    return _buffer.empty();
}

BufferReadStatus Buffer::get_line(std::string* line) {
    if (line == nullptr) {
        LOG(WARNING) << "[Buffer::get_line]: nullptr";
        return BufferReadStatus::READ_ERROR;
    }
    if (_cursor > _buffer.size()) {
        LOG(WARNING) << "[Buffer::get_line]: _cursor out of bound";
        line->clear();
        return BufferReadStatus::READ_OUTBOUND;
    }
    if (_cursor == _buffer.size() || _buffer.empty()) {
        line->clear();
        return BufferReadStatus::READ_END;
    }

    uint64_t len = _buffer.size();
    uint64_t i = 0;
    bool read_new_line = false;

    // try to find new line's end
    for (i = _cursor; i < len - 1; ++i) {
        if (_buffer[i] == 13 && _buffer[i + 1] == 10) {
            read_new_line = true;
            break;
        }
    }
    if (!read_new_line) {
        // can not find a new line
        line->clear();
        return BufferReadStatus::READ_FAIL;
    }
    // _cursor may be equal to i, this will set line to be a empty string
    line->assign(&_buffer[_cursor], &_buffer[i]);

    // ignore the \r and \n
    for (_cursor = i; _cursor < len; ++_cursor) {
        if (_buffer[_cursor] != 13 && _buffer[_cursor] != 10) {
            break;
        }
    }
    if (_cursor == len) {
        // reach the buffer's end
        return BufferReadStatus::READ_END;
    }
    return BufferReadStatus::READ_SUCCESS;
}

}
