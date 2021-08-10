#include "src/net/Channel.h"
#include <poll.h>

const int Channel::noneEvent = 0;
const int Channel::readEvent = POLLIN | POLLPRI;
const int Channel::writeEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) 
    : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    eventHanding_(false),
    addedToLoop_(false) {
}

Channel::~Channel() {
}

void Channel::setReadCallback(EventCallback cb) {
    readCallback_ = cb;
}

void Channel::setWriteCallback(EventCallback cb) {
    writeCallback_ = cb;
}

void Channel::setCloseCallback(EventCallback cb) {
    closeCallback_ = cb;
}

void Channel::setErrorCallback(EventCallback cb) {
    errorCallback_ = cb;
}

int Channel::fd() const {
    return fd_;
}

int Channel::events() const {
    return events_;
}

int Channel::index() const {
    return index_;
}

void Channel::setIndex(int index) {
    index_ = index;
}

void Channel::setRevents(int revents) {
    revents_ = revents;
}

void Channel::enableReading() {
    events_ |= readEvent;
    update();
};

void Channel::disableReading() {
    events_ &= ~readEvent;
    update();
}

void Channel::enableWriting() {
    events_ |= writeEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~writeEvent;
    update();
}

void Channel::disableAll() {
    events_ = noneEvent;
    update();
}

bool Channel::isReading() const {
    return events_ & readEvent;
}

bool Channel::isWriting() const {
    return events_ & writeEvent;
}

bool Channel::isNoneEvent() const {
    return events_ & noneEvent;
}

void Channel::update() {
    addedToLoop_ = true;
    loop_->updateChannel(this); // 调用 EventLoop 中的更新函数
}

void Channel::remove() {
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent() {

    eventHanding_ = true;
    if(revents_ & (POLLIN | POLLPRI)) {
        if(readCallback_)
            readCallback_();
    }

    if(revents_ & POLLOUT) {
        if(writeCallback_)
            writeCallback_();
    }

    if((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if(closeCallback_) 
            closeCallback_();
    }

    if(revents_ & (POLLERR | POLLNVAL)) {
        if(errorCallback_)
            errorCallback_();
    }
    eventHanding_ = false;
}