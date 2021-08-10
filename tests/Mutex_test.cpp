#include "src/base/Mutex.h"
#include "src/base/Timestamp.h"
#include "src/base/Thread.h"
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

MutexLock g_mutex;
vector<int> g_vec;

const int kCount = 1000*1000*10;

void taskWoutMutex() {
    for(int i=0; i<kCount; i++) {
        g_vec.push_back(i);
    }
}

void task() {
    for(int i=0; i<kCount; i++) {
        MutexGurad lock(g_mutex);
        g_vec.push_back(i);
    }
}

int64_t timeUsed(Timestamp& start) {
    return Timestamp::now().microSecondsSinceEpoch() 
            - start.microSecondsSinceEpoch();
}

int main()
{

    const int kThread = 8;

    /* 单线程不加锁执行 task 时间 */
    Timestamp start(Timestamp::now());
    taskWoutMutex();
    cout << "single thread without lock " << timeUsed(start) << endl;

    /* 单线程加锁执行 task 时间 */
    start = Timestamp::now();
    task();
    cout << "single thread with lock " << timeUsed(start) << endl;
    
    for(int nthreads=1; nthreads<kThread; nthreads++) {
        /* 使用智能指针可以防止内存泄漏，
            当该对象被销毁时，会在其析构函数中删除关联的原始指针*/
        vector<unique_ptr<Thread>> threads;
        g_vec.clear();
        start = Timestamp::now();

        /* 测试加锁后不同线程数同时修改临界区的效率 */
        for(int i=0; i<nthreads; i++) {
            threads.emplace_back(new Thread(&task));
            threads.back()->start();
        }
        for(int i=0; i<nthreads; i++) {
            threads[i]->join();
        }
        cout << nthreads << " thread with lock " << timeUsed(start) << endl;
    }

    return 0;
}