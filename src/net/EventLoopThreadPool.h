#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads);
    void start();

    EventLoop* getNextLoop();

    bool started() const;

private:
    EventLoop* baseLoop_; // Acceptor 所属的 EventLoop
    bool started_;
    int numThreads_; // 线程数
    unsigned int next_; // 下一个接收新连接的线程编号
    std::vector<std::unique_ptr<EventLoopThread>> threads_; // 所有 io 线程
    std::vector<EventLoop*> loops_; // 所有 io 线程的 EventLoop
};

#endif