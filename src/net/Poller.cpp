#include "src/net/Poller.h"
#include "src/net/Channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

Poller::Poller(EventLoop* loop) 
    : loop_(loop),
    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(16) {
}

Poller::~Poller() {
    close(epollfd_);
}

/* 封装 epoll_wait， 将有事件产生的 channel 加入到 EventLoop 的 activateChannels */
Timestamp Poller::poll(int timeoutMs, ChannelList* activateChannels) {
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs); // &*events_.begin() 取 vector 容器的首地址
    Timestamp now(Timestamp::now());

    int savedErrno = errno;
    /* 有事件 */
    if(numEvents > 0) {
        for(int i = 0; i < numEvents; i++) {
            Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->setRevents(events_[i].events);
            activateChannels->push_back(channel);
        }

        /* 对 events 进行扩容 */
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size()*2);
        }

    } else if (numEvents == 0) {
        /* 无事件发生 */
    } else {
        /* 错误 */
        if(savedErrno != EINTR) {
            perror("Poll error");
        }
    }
    
    return now;
}

/* 更新 epollfd 上关注的描述符 */
void Poller::updateChannel(Channel* channel) {
    const int index = channel->index();
    /* 若 fd 状态为待添加或者已从 epollfd 上删除，需要更新，则将 fd 添加到 epollfd 上 */
    if(index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if(index == kNew) {
            channels_[fd] = channel;
        } else {} //若为 deleted 状态则以及在 channels_ 里面，不做任何操作

        update(EPOLL_CTL_ADD, channel);
        channel->setIndex(kAdded);
    } else { // 若 fd 状态为已经在 epollfd 上，需要更新
        // int fd = channel->fd();
        if(channel->isNoneEvent()) { // 若 channel 没有关注的事件，则从 epollfd 上拿下
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else { // 否则修改 epollfd 上的 fd
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

/* 从 channels_ 中移除 channel */
void Poller::removeChannel(Channel* channel) {

    int fd = channel->fd();
    channels_.erase(fd);
    if(channel->index() == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kAdded);
}

/* 封装 epoll_ctl 所需的相关操作 */
void Poller::update(int operation, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();

    if(epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        perror("epoll_ctl update error");
    }
}