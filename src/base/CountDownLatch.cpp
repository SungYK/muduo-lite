#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) 
    : mutex_(),
    cond_(mutex_),
    count_(count) {
}

void CountDownLatch::wait() {
    MutexGurad lock(mutex_);
    while(count_ > 0) {
        cond_.wait();
    }
}

void CountDownLatch::countDown() {
    MutexGurad lock(mutex_);
    count_--;
    if(count_ == 0) {
        cond_.notifyAll();
    }
}

int CountDownLatch::getCount() const {
    MutexGurad lock(mutex_);
    return count_;
}
