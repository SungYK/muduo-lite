#include "src/base/Thread.h"
// #include <thread>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>
#include <string>

using namespace std;

pid_t tid() {
    return static_cast<pid_t>(syscall(SYS_gettid));
};

void Task_1() {
    cout << "tid=" << tid() << endl;
}

void Task_2(int x, string info) {
    cout << "tid=" << tid() << " arg=" << x << " info:" << info << endl;;
}

int main()
{
    Task_1(); // 主线程
    Thread t1(Task_1);
    t1.start();
    t1.join();

    Thread t2(std::bind(&Task_2, 42, "Thread 2"));
    t2.start();
    t2.join();

    return 0;
}
