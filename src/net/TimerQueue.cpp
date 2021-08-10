#include "src/net/TimerQueue.h"
#include "src/net/EventLoop.h"

#include <sys/timerfd.h> 
#include <strings.h>
#include <unistd.h>



struct timespec howMuchTimeFromNow(Timestamp when) {
    int64_t ms = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if(ms < 100)
        ms = 100;
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(ms / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((ms % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

int createTimerfd() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        perror("create timerfd error");
    }
    return timerfd;
}

void resetTimerfd(int timerfd, Timestamp expiration) {
    struct itimerspec newVal;
    struct itimerspec oldVal;
    bzero(&newVal, sizeof(newVal));
    bzero(&oldVal, sizeof(oldVal));
    newVal.it_value = howMuchTimeFromNow(expiration);
    int ret = timerfd_settime(timerfd, 0, &newVal, &oldVal);
    if(ret) {
        perror("resetTimerfd error");
    }
}

void readTimerfd(int timerfd) {
    uint64_t buf;
    ssize_t n = read(timerfd, &buf, sizeof(buf));
    if(n != sizeof(buf)) {
        perror("readTimerfd error");
    }
}

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(createTimerfd()),
      timerfdChannel_(new Channel(loop, timerfd_)),
      timers_() {
    
    /* 关注 timerfd 的读事件 */
    timerfdChannel_->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_->enableReading();
}

TimerQueue::~TimerQueue() {
    timerfdChannel_->disableAll();
    timerfdChannel_->remove();
    close(timerfd_);
    for(Entry timer : timers_)
        delete timer.second;
}

Timer* TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
    
    Timer* timer = new Timer(cb, when, interval);

    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));

    return timer;
}

void TimerQueue::cancel(Timer* timer) {
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timer));
}

void TimerQueue::addTimerInLoop(Timer* timer) {

    /* 返回插入后按时间排序的 timers_ 的第一个定时器否改变 */
    bool earlistChanged = insert(timer);

    /* 如果改变了则需要调整 timerfd_ 的触发事件为新的第一个定时器的超时时间 */
    if(earlistChanged) {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(Timer* timer) {
    
    auto it = activeTimers_.find(ActiveTimer(timer, timer->sequence()));
    if(it != activeTimers_.end()) {
        timers_.erase(Entry(it->first->expiration(), it->first));
        delete it->first;
        activeTimers_.erase(it);
    }
}

void TimerQueue::handleRead() {
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_); // 处理 POLLIN 事件，避免忙等

    Expired expired = getExpired(now);

    /* 执行超时定时器的回调 */
    for(const Entry& it : expired) {
        it.second->run();
    }

    reset(expired);
}

TimerQueue::Expired TimerQueue::getExpired(Timestamp now) {
    Expired expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto end = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    /* 在 ActiveTimers_ 中也删除过期定时器 */
    for(const Entry& it : expired) {
        activeTimers_.erase(ActiveTimer(it.second, it.second->sequence()));
    }
    return expired;
}


void TimerQueue::reset(const Expired& expired) {
    Timestamp nextExpired;
    Timestamp now(Timestamp::now());

    /* 重启周期定时的定时器，删除过期的一次性定时器 */
    for(const Entry& it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        if(it.second->repeat()) {
            it.second->restart();
            insert(it.second);
        } else {
            delete it.second;
        }
    }

    /* timerfd_ 与最近到期定时器绑定 */
    if(!timers_.empty())
        nextExpired = timers_.begin()->second->expiration();

    if(nextExpired.valid())
        resetTimerfd(timerfd_, nextExpired);

}


bool TimerQueue::insert(Timer* timer) {
    bool earlistChanged = false;
    Timestamp when = timer->expiration();
    auto it = timers_.begin();
    if(it == timers_.end() || when < it->first)
        earlistChanged = true;

    timers_.insert(Entry(when, timer));
    activeTimers_.insert(ActiveTimer(timer, timer->sequence()));

    return earlistChanged;
}