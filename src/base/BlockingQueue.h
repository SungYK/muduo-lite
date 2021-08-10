#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include "src/base/Mutex.h"
#include "src/base/Condition.h"
#include <deque>

template<typename T>
class BlockingQueue {
public:
    BlockingQueue()
    : mutex_(),
    notEmpty_(mutex_),
    queue_(),
    using_(true) {

    }

    void put(T& x) {
        MutexGurad lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify();
    }

    T take() {
        MutexGurad lock(mutex_);
        /* wait() 的同时while循环检查是否为空，防止唤醒操作的影响 */
        while (queue_.empty() && using_) {
            notEmpty_.wait();
        }
        if(using_) {
            T front = queue_.front();
            queue_.pop_front();
            return front;
        }
        return NULL;
        
    }

    size_t size() const {
        MutexGurad lock(mutex_);
        return queue_.size();
    }

    /* 用于在需要销毁队列时，停止阻塞 */
    void notify() {
        using_ = false;
        notEmpty_.notifyAll();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T> queue_;
    bool using_;
};

#endif