#include "src/net/Buffer.h"

const char* Buffer::peek() const {
    return buf_.c_str();
}

int Buffer::readableBytes() {
    return static_cast<int>(buf_.size());
}

void Buffer::retrieve(int len) {
    buf_ = buf_.substr(len, buf_.size());
}

void Buffer::append(const std::string& buf) {
    buf_.append(buf);
}

std::string Buffer::retrieveAsString(int len) {
    std::string ret(peek(), len);
    retrieve(len);
    return ret;
}

std::string Buffer::retrieveAllAsString() {
    std::string ret(peek(), readableBytes());
    retrieve(readableBytes());
    return ret;
}