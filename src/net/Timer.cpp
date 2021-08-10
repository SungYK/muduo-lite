#include "src/net/Timer.h"

atomic_int64_t Timer::numCreated_;

Timer::Timer(TimerCallback cb, Timestamp when, double interval) 
    : callback_(cb),
    expiration_(when), 
    interval_(interval),
    repeat_(interval_ > 0.0),
    sequence_(numCreated_++) {
}

void Timer::run() {
    callback_();
}

Timestamp Timer::expiration() const {
    return expiration_;
}

bool Timer::repeat() const {
    return repeat_;
}

int64_t Timer::sequence() const {
    return sequence_;
}

void Timer::restart() {
    if(repeat_) {
        expiration_ = Timestamp::nowAfter(interval_);
    } else {
        expiration_ = Timestamp(); // 返回一个无效的时间戳
    }
}


