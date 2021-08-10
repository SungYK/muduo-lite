#ifndef CHANNEL_H
#define CHANNEL_H

#include "src/net/EventLoop.h"
#include <functional>

class EventLoop;

class Channel {
public:
    typedef std::function<void()> EventCallback;
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(); // 处理事件
    
    /* 设置事件回调函数 */
    void setReadCallback(EventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setCloseCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);

    int fd() const;
    int events() const;
    int index() const;
    void setIndex(int index);
    void setRevents(int revents); // 用于 poller 设置返回事件

    /* 设置关注的事件 */
    void enableReading();
    void disableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    bool isWriting() const;
    bool isReading() const;
    bool isNoneEvent() const;

    void remove(); // 从 Poller channels_ 中移除

private:
    void update(); // 更新 channel 状态

    /* Channel 可能关注的事件 */
    static const int noneEvent; // 0
    static const int readEvent; // POLLIN | POLLPRI
    static const int writeEvent; // POLLOUT

    EventLoop* loop_; // Channel 所属的 EventLoop
    int fd_; // Channel 管理的 fd
    int events_; // Channel 关注的事件
    int revents_; // poll 返回的事件
    int index_; // poller 使用，描述当前 Channel 中 fd 在 epollfd 上状态 new(-1) added(1) deleted(2)

    bool eventHanding_;
    bool addedToLoop_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

};

#endif