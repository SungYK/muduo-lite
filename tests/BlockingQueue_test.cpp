#include "src/base/BlockingQueue.h"
#include "src/base/CountDownLatch.h"
#include "src/base/Thread.h"

#include <iostream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

const int nthreads = 3;
CountDownLatch g_latch(nthreads);
BlockingQueue<string> g_queue;

void task(int tIndex) {
    g_latch.countDown();
    bool running = true;
    while (running) {
        string d = g_queue.take();
        cout << tIndex << ":" << d << endl;
        running = (d != "stop");
    }    
}

int main()
{

    const int times = 100;
    vector<unique_ptr<Thread>> threads;

    for(int i=1; i<=nthreads; i++) {
        threads.emplace_back(new Thread(bind(&task, i)));
    }

    for(int i=0; i<nthreads; i++) {
        threads[i]->start();
    }

    g_latch.wait();
    cout << "all threads started" << endl;
    for(int i=0; i<times; i++) {
        string d = "test from main thread";
        g_queue.put(d);
    }

    for (size_t i = 0; i < threads.size(); i++) {
        string d = string("stop");
        g_queue.put(d);
    }

    for(int i=0; i<nthreads; i++) {
        threads[i]->join();
    }
    
    return 0;
}