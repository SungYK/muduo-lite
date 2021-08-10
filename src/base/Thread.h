#ifndef THREAD_H
#define THREAD_H

#include "src/base/CountDownLatch.h"
#include <functional>
#include <pthread.h>
#include <string>

class Thread {
public:
    typedef std::function<void ()> Task; // 定义 Task 为线程任务函数指针
    Thread(Task t);
    ~Thread();

    void start();
    int join();
    bool started();
    pid_t tid();

private:

    Task task_;
    bool started_;
    bool joined_;
    pthread_t pthreadId_; // 进程内唯一id，不同进程可能相同
    pid_t tid_; // 全局唯一 id
    
    CountDownLatch latch_; // 线程计数门闩，用于统一线程执行

};

#endif