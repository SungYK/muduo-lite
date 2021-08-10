#ifndef BUFFER_H
#define BUFFER_H

#include <string>

class Buffer {
public:
    Buffer() = default;
    ~Buffer() = default;
    const char* peek() const;
    int readableBytes();
    void retrieve(int len);
    void append(const std::string& buf);

    std::string retrieveAsString(int len);
    std::string retrieveAllAsString();

private:
    std::string buf_;

};

#endif