#ifndef EVENTLOOP_THREAD_H
#define EVENTLOOP_THREAD_H

#include "src/base/Condition.h"
#include "src/base/Mutex.h"
#include "src/base/Thread.h"

class EventLoop;

class EventLoopThread {
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();


private:
    void task();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    
};

#endif