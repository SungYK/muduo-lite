#include "src/net/Timer.h"

#include <iostream>

using namespace std;

void timer_cb() {
    cout << "Timer expired" << endl;
}

int main() {

    /* 3秒定时器 */
    Timestamp timestamp = Timestamp::nowAfter(3);
    Timer t1(timer_cb, timestamp, 0);

    while (Timestamp::now() < t1.expiration());
    t1.run();
    cout << t1.sequence() << endl;

    /* 间隔 1 秒重复定时器 */
    timestamp = Timestamp::nowAfter(3);
    Timer t2(timer_cb, timestamp, 1);

    int times = 10;
    while(times--) {
        while (Timestamp::now() < t2.expiration());
        t2.run();
        t2.restart();
    }
    cout << t2.sequence() << endl;
    return 0;
}

