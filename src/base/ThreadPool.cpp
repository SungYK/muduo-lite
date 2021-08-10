#include "src/base/ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool()
    : running_(false) {
}

ThreadPool::~ThreadPool() {
    if(running_)
        stop();
}

void ThreadPool::start(int numThreads) {
    running_ = true;
    threads_.reserve(numThreads);
    for(int i=0; i<numThreads; i++) {
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this)));
        threads_[i]->start();
    }
}

void ThreadPool::stop() {
    running_ = false;
    queue_.notify();
    for(size_t i=0; i<threads_.size(); i++) {
        threads_[i]->join();
    }   

}

size_t ThreadPool::queueSize() const {
    return queue_.size();
}

void ThreadPool::run(Task task) {
    
    /* 单线程 */
    if(threads_.empty()) {
        task();
    } else {
        if(!running_) return;  
        queue_.put(task); // 向任务队列添加任务，生产者
    }
}

void ThreadPool::runInThread() {
    try {
        /* wait 任务队列不为空，消费者 */
        while (running_) {
            Task task = queue_.take();
            if(task)
                task();
        }
    } catch(...) {
        std::cout << "error in thread pool" << std::endl;
        throw;
    }
}



