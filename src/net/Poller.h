#ifndef POLLER_H
#define POLLER_H

#include "src/net/EventLoop.h"
#include <map>

class Poller {
public:
    Poller(EventLoop* loop);
    ~Poller();

    typedef std::vector<Channel*> ChannelList;
    Timestamp poll(int timeoutMs, ChannelList* activateChannels);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void update(int operation, Channel* channel);

private:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_; // 所有已创建的 channel，存储 fd 以及对应 channel*

    EventLoop* loop_;

    typedef std::vector<struct epoll_event> EventList;
    int epollfd_;
    EventList events_;
};

#endif