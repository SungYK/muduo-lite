#ifndef CONDITION_H
#define CONDITION_H

#include "src/base/Mutex.h"
#include <pthread.h>
#include <iostream>

class Condition {
public:
    Condition(MutexLock& mutex) : mutex_(mutex) {
        pthread_cond_init(&cond_, NULL);
    };

    ~Condition() {
        pthread_cond_destroy(&cond_);
    };

    void wait() {
        pthread_cond_wait(&cond_, mutex_.mutexId());
    };

    void notify() {
        pthread_cond_signal(&cond_);
    };

    void notifyAll() {
        pthread_cond_broadcast(&cond_);
    };

private:
    MutexLock& mutex_;
    pthread_cond_t cond_;
};

#endif