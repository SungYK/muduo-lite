#include "src/net/EventLoop.h"
#include "src/net/Poller.h"
#include "src/net/TimerQueue.h"
#include "src/net/Channel.h"
#include "src/base/CurrentThread.h"

#include <unistd.h>
#include <iostream>

int createEventFd() {
    return 1;
}

EventLoop::EventLoop()
    : looping_(false),
    quit_(false),
    eventHanding_(false),
    callingPendingFuncs_(false),
    threadId_(CurrentThread::tid()),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventFd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    currentActivateChannel(NULL) {

        /* 初始化唤醒 channel，设置唤醒回调为 handleRead，关注 wakeupfd_ 的读事件 */
        wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
        wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    close(wakeupFd_);
}

void EventLoop::loop() {
    looping_ = true;
    while (!quit_) {
        activateChannels_.clear();
        poller_->poll(1000, &activateChannels_);
        eventHanding_ = true;
        for(Channel* channel : activateChannels_) {
            currentActivateChannel = channel;
            currentActivateChannel->handleEvent();
        }
        currentActivateChannel = NULL;
        eventHanding_ = false;
        doPendingFuncs(); // 执行 io 线程任务队列中挂载的任务
    }

    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;

    /* 若不是在 IO 线程中调用，则需要唤醒 poll 阻塞 */
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Func cb) {
    // std::cout<<"here1"<<std::endl;
    if(isInLoopThread()) {
        cb();
    } else { // 若不是在 IO 线程中调用，则需要加到 IO 线程任务队列中
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Func cb) {
    /* 加入任务到 io 线程任务队列 */
    {
        MutexGurad lock(mutex_);
        pendingFuncs_.push_back(cb);
    }

    /* 当不是在 IO 线程调用此函数时，需要唤醒阻塞
       或是 IO 线程调用此函数但 io 线程正在执行 doPendingFuncs 时也需要唤醒
       因为 doPendingFuncs 后会再次进入阻塞 */
    if(!isInLoopThread() || callingPendingFuncs_) {
        wakeup();
    }

} 

void EventLoop::updateChannel(Channel* channel) {

    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    while(eventHanding_);
    poller_->removeChannel(channel);
} 

void EventLoop::wakeup() {
    uint64_t one = 1;
    /* 向 wakupFd_ 写入使其产生 POLLIN 事件，以此唤醒 poll 阻塞 */
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)) {
        perror("EventLoop::wakeup error");
    }
    std::cout << "poll wakeup" << std::endl;
}

/* 处理唤醒事件，防止进入 busy loop */
void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)) {
        perror("EventLoop::handleRead error");
    }
}

/* 执行 IO 线程中任务队列中的事件 */
void EventLoop::doPendingFuncs() {
    callingPendingFuncs_ = true;
    std::vector<Func> funcs;
    {
        MutexGurad lock(mutex_);
        funcs.swap(pendingFuncs_);
    }
    for(auto it=funcs.begin(); it!=funcs.end(); it++) {
        (*it)();
    }
    callingPendingFuncs_ = false;
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == CurrentThread::t_cachedTid;
}

/* 定时器任务相关函数 */
Timer* EventLoop::runAt(Timestamp time, TimerCallback cb) {
    return timerQueue_->addTimer(cb, time, 0.0);
}

Timer* EventLoop::runAfter(double delay, TimerCallback cb) {
    Timestamp time(Timestamp::nowAfter(delay));
    
    return runAt(time, cb);
}

Timer* EventLoop::runEvery(double interval, TimerCallback cb) {
    Timestamp time(Timestamp::nowAfter(interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::cancelTimer(Timer* timer) {
    timerQueue_->cancel(timer);
}

