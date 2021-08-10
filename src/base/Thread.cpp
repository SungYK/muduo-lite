#include "Thread.h"
#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <iostream>


namespace CurrentThread {
    pid_t tid() {
        return static_cast<pid_t>(syscall(SYS_gettid));
    };
}

/* 封装线程任务，方便转换为 void*，并在入口中调用 */
struct ThreadData {
    typedef Thread::Task Task;
    Task task_;
    pid_t* tid_; // 传入 tid 指针，在线程启动后修改
    CountDownLatch* latch_;

    ThreadData(Task t, pid_t* tid, CountDownLatch* latch)
    : task_(t),
    tid_(tid),
    latch_(latch)
    {}
    
    void runInThread() {
        *tid_ = CurrentThread::tid();
        latch_->countDown(); // 计数器减为0，通知主线程已经启动
        task_();
    }
};

/* 线程任务函数统一入口 */
void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}


Thread::Thread(Task t) 
    : task_(t), 
    started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    latch_(1)
{}

Thread::~Thread() {
    if(started_ && !joined_)
        pthread_detach(pthreadId_);
}

void Thread::start() {
    
    assert(!started_);
    started_ = true;
    struct ThreadData* data = new ThreadData(task_, &tid_, &latch_);
    if(pthread_create(&pthreadId_, NULL, &startThread, data)) {
        started_ = false;
        perror("pthread create error");
        exit(1);
    } else {
        latch_.wait(); // 等待子线程开始执行
    }

}

int Thread::join() {

    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

pid_t Thread::tid() {
    return tid_;
}
