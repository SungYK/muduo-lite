#include "src/net/EventLoopThreadPool.h"
#include "src/net/EventLoopThread.h"
#include "src/net/EventLoop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop) 
    : baseLoop_(baseLoop), 
    started_(false), 
    numThreads_(0), 
    next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start() {

    for(int i=0; i<numThreads_; i++) {
        
        EventLoopThread* elt = new EventLoopThread;
        
        threads_.push_back(std::unique_ptr<EventLoopThread>(elt));

        loops_.push_back(elt->startLoop()); // 启动 loop 并在出现场存储 *loop
        // printf("here%d\n", i);
    }
}

/* 循环选择线程池中 loop 接收新连接 */
EventLoop* EventLoopThreadPool::getNextLoop() {
    EventLoop* loop = baseLoop_;
    if(!loops_.empty()) {
        loop = loops_[next_];
        next_++;

        if(next_ == loops_.size())
            next_ = 0;
    }
    return loop;
}

void EventLoopThreadPool::setThreadNum(int numThreads) {
    numThreads_ = numThreads;
}

bool EventLoopThreadPool::started() const {
    return started_;
}