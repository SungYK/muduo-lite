#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "src/base/BlockingQueue.h"
#include "src/base/Thread.h"
#include <functional>
#include <vector>
#include <memory>

/* 生产者消费者模型 */
class ThreadPool {
public:
    typedef std::function<void ()> Task;
    ThreadPool();
    ~ThreadPool();

    void start(int numThreads);
    void stop();

    size_t queueSize() const;

    void run(Task t);

private:
    void runInThread();
    BlockingQueue<Task> queue_; // 任务队列
    std::vector<std::unique_ptr<Thread>> threads_; // 线程池
    bool running_;
};

#endif