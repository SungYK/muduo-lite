#include "src/net/EventLoopThread.h"
#include "src/net/EventLoop.h"

EventLoopThread::EventLoopThread() 
    : loop_(NULL), 
    exiting_(false), 
    thread_(std::bind(&EventLoopThread::task, this)), 
    mutex_(), 
    cond_(mutex_) {
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if(loop_ != NULL) {
        loop_->quit();
        thread_.join();
    }
}

/* 在子线程中启动 loop，并得到子线程线程 *loop 给主线程 */
EventLoop* EventLoopThread::startLoop() {
    
    thread_.start();
    EventLoop* loop = NULL;
    {
        MutexGurad lock(mutex_);
        while(loop_ == NULL) {

            cond_.wait();
            

        }
        loop = loop_;
    }
    return loop;
}

/* 在子线程中运行 loop，创建成功后通知主线程取得 *loop */
void EventLoopThread::task() {
    EventLoop loop;
    {
        MutexGurad lock(mutex_);
        
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();

    MutexGurad lock(mutex_);
    loop_ = NULL;
}