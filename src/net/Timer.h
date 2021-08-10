#ifndef TIMER_H
#define TIMER_H

#include "src/base/Timestamp.h"
#include <functional>
#include <atomic>



class Timer {
public:
    typedef std::function<void()> TimerCallback;
    
    Timer(TimerCallback cb, Timestamp when, double interval);

    void run();
    Timestamp expiration() const;
    bool repeat() const;
    int64_t sequence() const;

    void restart();

    
    

private:
    TimerCallback callback_; // 定时器回调
    Timestamp expiration_;  // 定时器过期时间戳
    const double interval_; // 定时器间隔，表示间隔多久定时器触发一次，不重复则为 0
    const bool repeat_; // 是否重复
    int64_t sequence_;
    static atomic_int64_t numCreated_; // 记录创建的定时器个数，使用原子类型，
};


#endif