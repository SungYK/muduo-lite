#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <src/base/Timestamp.h>
#include <src/base/Mutex.h>
#include "src/net/Channel.h"
#include "src/net/TimerQueue.h"

#include <functional>
#include <vector>
#include <memory>

class Channel; /* 管理 fd 以及所关注的事件 */
class Poller; /* epoll 操作封装 */
class TimerQueue; /* 定时器队列管理 */

class EventLoop {
public:
    typedef std::function<void()> Func;
    typedef std::function<void()> TimerCallback;

    EventLoop();
    ~EventLoop();

    void loop(); // 开始循环
    void quit(); // 退出循环

    void runInLoop(Func cb); // 在循环中处理其他事务，立即唤醒阻塞，立即调用回调
    void queueInLoop(Func cb); // 在循环中处理其他事务，等待阻塞结束后再执行回调

    /* 定时任务 */
    Timer* runAt(Timestamp time, TimerCallback cb);
    Timer* runAfter(double delay, TimerCallback cb);
    Timer* runEvery(double interval, TimerCallback cb);

    void cancelTimer(Timer* timer);

    void wakeup(); // 唤醒 poll 阻塞
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool isInLoopThread() const; // 判断当前是否再 IO loop 线程中


private:
    typedef std::vector<Channel*> ChannelList;

    void handleRead(); // 处理 wakeupfd 唤醒事件
    void doPendingFuncs(); // 处理放到 IO 线程任务队列里的任务

    bool looping_;
    bool quit_;
    bool eventHanding_;
    bool callingPendingFuncs_;

    const pid_t threadId_;

    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    
    int wakeupFd_; // 用于唤醒 loop 阻塞的文件描述符
    std::unique_ptr<Channel> wakeupChannel_; // 管理 wakeupFd_ 的 channel

    ChannelList activateChannels_;
    Channel* currentActivateChannel;

    mutable MutexLock mutex_;
    std::vector<Func> pendingFuncs_;

};

#endif