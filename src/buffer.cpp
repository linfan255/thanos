#include "easylogging++.h"
#include "buffer.h"
#include "common_type.h"

namespace thanos {

Buffer::Buffer() : _buffer(), _cursor(0) {}

Buffer::~Buffer() = default;

Buffer::Buffer(const Buffer& other) : 
        _buffer(other._buffer), 
        _cursor(other._cursor) {}

void Buffer::show_content() const {
    std::string content = "";
    content.assign(_buffer.begin(), _buffer.end());
    LOG(DEBUG) << "BUFFER CONTENT:";
    LOG(INFO) << content;
}

Buffer& Buffer::operator=(const Buffer& other) {
    if (this == &other) {
        return *this;
    }
    _cursor = other._cursor;
    _buffer = other._buffer;
    return *this;
}

Buffer::Buffer(Buffer&& other) :
        _buffer(std::move(other._buffer)),
        _cursor(std::move(other._cursor)) {}

Buffer& Buffer::operator=(Buffer&& other) {
    if (this == &other) {
        return *this;
    }
    _cursor = std::move(other._cursor);
    _buffer = std::move(other._buffer);
    return *this;
}

bool Buffer::dump(Buffer* other) {
    if (other == nullptr) {
        LOG(WARNING) << "[Buffer::dump]: dump to no where";
        return false;
    }

    if (this == other) {
        return true;
    }

    other->_cursor = std::move(_cursor);
    other->_buffer = std::move(_buffer);
    _cursor = 0;
    return true;
}

uint64_t Buffer::size() const {
    return _buffer.size();
}

void Buffer::push_nbytes(const char* buffer, uint64_t nbytes) {
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

bool Buffer::read_until_end(std::string* content) {
    if (content == nullptr) {
        LOG(WARNING) << "[Buffer::read_until_end]: parameter is nullptr";
        return false;
    }
    if (_cursor > _buffer.size()) {
        LOG(WARNING) << "[Buffer::read_until_end]: _cursor out of bound";
        return false;
    }

    uint64_t len = _buffer.size();
    content->assign(&_buffer[_cursor], &_buffer[len]);
    return true;
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
    for (_cursor = i; _cursor < len && _cursor < i + 2; ++_cursor) {
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
