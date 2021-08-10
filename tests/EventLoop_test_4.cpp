/* 测试 TimerQueue */
#include "src/net/EventLoop.h"
#include "src/base/Thread.h"
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

EventLoop* g_loop;
int cnt = 0;

void print(string msg) {
  cout << msg << endl;
  if (++cnt == 20) {
    g_loop->quit();
  }
}

void cancel(Timer* timer, string name) {
    g_loop->cancelTimer(timer);
    printf("cancelled %s at %s\n", name.c_str(), Timestamp::now().toString().c_str());
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    
    loop.runAfter(1, bind(print, "once1"));
    loop.runAfter(1.5, bind(print, "once1.5"));
    loop.runAfter(2.5, bind(print, "once2.5"));
    loop.runAfter(3.5, bind(print, "once3.5"));
    Timer* timer45 = loop.runAfter(4.5, bind(print, "once4.5"));
    loop.runAfter(4.2, bind(cancel, timer45, "timer45"));
    loop.runAfter(4.8, bind(cancel, timer45, "timer45"));
    loop.runEvery(2, bind(print, "every2"));
    Timer* t3 = loop.runEvery(3, bind(print, "every3"));
    loop.runAfter(9.001, bind(cancel, t3, "t3"));

    loop.loop();
    print("main loop exits");
    loop.loop();


    return 0;
}
