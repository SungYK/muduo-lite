/* 测试在 IO 线程中执行其他任务，测试唤醒 poll 阻塞，测试跨线程调用在 IO 线程中执行其他任务*/
#include <src/net/EventLoop.h>
#include <src/base/Thread.h>
#include <src/base/CurrentThread.h>
#include <unistd.h>
#include <stdio.h>

EventLoop *g_loop;
int g_flag = 0;

void run4() {
    printf("run4(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->quit();
}

void run3() {
    printf("run3(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->queueInLoop(run4);
    // g_loop->runAfter(1, run4);
    g_flag = 3;
}

void run2() {
    printf("run2(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->queueInLoop(run3);
}

void run1() {
    g_flag = 1;
    printf("run1(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->runInLoop(run2);
    g_flag = 2;
}

void run() {
    sleep(2); // 等待 loop 开始
    printf("run(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
    g_loop->runInLoop(run1);
}

int main()
{
    printf("main(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);

    EventLoop loop;
    g_loop = &loop;

    Thread t(run);
    t.start();

    loop.loop();
    t.join();

    printf("main(): pid = %d, tid = %d, flag = %d\n", getpid(), CurrentThread::tid(), g_flag);
}