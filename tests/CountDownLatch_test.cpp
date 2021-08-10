#include "src/base/CountDownLatch.h"
#include "src/base/Thread.h"
#include "src/base/Mutex.h"
#include <unistd.h>
#include <vector>
#include <memory>
#include <string>

using namespace std;

const int nthreads = 3;
CountDownLatch g_latch_1(1);
CountDownLatch g_latch_2(nthreads);
MutexLock g_mutex;
int g_int = 0;


void task_1(int n) {
    /* 等待计数门闩为 0 启动线程 */
    g_latch_1.wait();
    MutexGurad lock(g_mutex);
    g_int = n;
}

void task_2() {
    sleep(1);
    g_latch_2.countDown();
    MutexGurad lock(g_mutex);
    g_int--;
}

/* 所有子线程等待主线程 */
void test_1() {

    vector<unique_ptr<Thread>> threads;

    for(int i=1; i<=nthreads; i++) {
        threads.emplace_back(new Thread(bind(&task_1, i)));
    }

    /* 先启动子线程，阻塞等待主线程 */ 
    for(int i=0; i<nthreads; i++) {
        threads[i]->start();
    }

    
    cout << "wait main thread" << endl;
    sleep(3);
    cout << g_int << endl; // 0
    /* 等待主线程计数器减为0，子线程阻塞结束 */
    g_latch_1.countDown();

    for(int i=0; i<nthreads; i++) {
        threads[i]->join();
    }
}

/* 主线程等待所有子线程 */
void test_2() {
    vector<unique_ptr<Thread>> threads;

    for(int i=1; i<=nthreads; i++) {
        threads.emplace_back(new Thread(task_2));
    }

    cout << g_int << endl; // nthreads

    /* 启动子线程 */ 
    for(int i=0; i<nthreads; i++) {
        threads[i]->start();
    }

    /* 阻塞等待子线程 */
    g_latch_2.wait();
    cout << "wait child threads" << endl;
    {
        MutexGurad lock(g_mutex);
        cout << g_int << endl;
    }

    for(int i=0; i<nthreads; i++) {
        threads[i]->join();
    }    
}

int main()
{   
    test_1();

    g_int = nthreads;
    test_2();

    return 0;
}