#include "src/base/ThreadPool.h"
#include <unistd.h>
#include <iostream>

using namespace std;

void longTask(const int num) {
    cout << "long task: " << endl;
    usleep(100*1000);
}

void print() {
    cout << "run in thread\n" << endl;
}

void printString(const string str) {
    cout << str << endl;
}

void test_1(const int nThreads) {
    ThreadPool pool;
    pool.start(5);
    
    pool.run(print);
    pool.run(print);

    for(int i=0; i<5; i++) {
        pool.run(bind(printString, "run in "+to_string(i)));
    }

    CountDownLatch latch(1);
    pool.run(bind(&CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();

}

// TODO:
void test_2() {
    ThreadPool pool;
    pool.start(3);
    Thread t([&pool] {
        for(int i=0; i<20; i++) {
            pool.run(bind(longTask, i));
        }
    });
    t.start();
    
    pool.stop();

    t.join();

    pool.run(print);

}

int main()
{
    // /* 单线程 */
    // test_1(0);
    // /* 线程池仅一个线程 */
    // test_1(1);
    // /* 多线程 */
    // test_1(5);

    test_2();

    return 0;
}