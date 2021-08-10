#include "src/base/Thread.h"
#include "src/base/Condition.h"
#include "src/base/Mutex.h"
#include <unistd.h>
#include <iostream>

using namespace std;

int g_int;
MutexLock g_mutex;
Condition g_cond(g_mutex);

/* 占用临界区 3 秒 */
void lockAndUnlock() {
    
    g_mutex.lock();
    g_int = 1;
    sleep(3);
    g_mutex.unlock();
    g_cond.notifyAll();
    
}


void task() {

    {
        g_cond.wait();
        g_int = 2;
        g_mutex.unlock();
    }
    
}


int main()
{

    Thread t1(lockAndUnlock);
    t1.start();
    sleep(1);
    cout << g_int << endl;
    Thread t2(task);
    t2.start();
    
    t1.join();
    
    t2.join();
    cout << g_int << endl;

    return 0;
}