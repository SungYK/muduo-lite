#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "src/base/Mutex.h"
#include "src/base/Condition.h"

class CountDownLatch {
public:
    CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    /* mutable 关键字使得 mutex_ 在常函数中可修改 */
    mutable MutexLock mutex_;
    Condition cond_;
    int count_;
};

#endif