#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

class MutexLock {
public:
    MutexLock() {
        pthread_mutex_init(&mutexId_, NULL);
    }

    ~MutexLock() {
        pthread_mutex_destroy(&mutexId_);
    }

    void lock() {
        pthread_mutex_lock(&mutexId_);
    }

    void unlock() {
        pthread_mutex_unlock(&mutexId_);
    }

    pthread_mutex_t* mutexId() {
        return &mutexId_;
    }

private:
    
    pthread_mutex_t mutexId_;

};

/* 将 lock 和 unlock 封装到一个对象的生命周期中，构造对象时 lock， 调用析构函数时 unlock 
在局部作用域即可实现自动解锁
{
    MutexGuard(mutex);
    // 临界区操作
}
// 作用域结束调用析构函数，自动解锁
*/
class MutexGurad {
public:
    MutexGurad(MutexLock &mutex) : mutex_(mutex) {
        mutex_.lock();
    }

    ~MutexGurad() {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

#endif