#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include "src/base/Mutex.h"
#include "src/base/Timestamp.h"
#include "src/net/Timer.h"
#include "src/net/Channel.h"

#include <set>
#include <vector>

class EventLoop;
class Channel;

class TimerQueue {
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    typedef std::function<void()> TimerCallback;
    Timer* addTimer(TimerCallback cb, Timestamp when, double interval);
    void cancel(Timer* timer);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerList;
    typedef std::vector<Entry> Expired;
    
    void addTimerInLoop(Timer* timer);
    void cancelInLoop(Timer* timer);
    void handleRead(); // 处理timerfd时间，即定时器到期
    void reset(const Expired& expired); // 重置过期定时器中周期定时的，并使 timerfd_ 绑定最近的计时器时间

    bool insert(Timer* timer); // 在 timers_ 中插入定时器

    Expired getExpired(Timestamp now);

    EventLoop* loop_;
    const int timerfd_; // 定时器文件描述符
    Channel* timerfdChannel_; // 管理定时器文件描述符事件的 Channel
    TimerList timers_; // 按时间排序的定时器集合 timers_ 与 activeTimers 存储的timer一致
    ActiveTimerList activeTimers_; // 按照地址排顺序的定时器集合
    ActiveTimerList cancelingTimers_; // 被取消的定时器集合

};

#endif