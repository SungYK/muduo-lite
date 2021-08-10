参照 Muduo 库结构实现的高并发 TCP 网络库

# 编译
```
git clone https://github.com/SungYK/muduo-lite.git
./bulid.sh
```

# 结构
```
src
\-- base
    |-- BlockingQueue.h 无界阻塞队列（消费者生产者队列）
    |-- Condition.h 条件变量，与 Mutex 一同使用
    |-- CountDownLatch.{h,cpp} “倒计时门闩”同步
    |-- Mutex.h 互斥器
    |-- Thread.{h,cpp} 线程对象
    \-- ThreadPool.{h,cpp} 简单的固定大小线程池

\-- net
    |-- Acceptor.{h,cpp} 接受器，用于服务端接受连接
    |-- Buffer.{h,cpp} 缓冲区，非阻塞 IO 必备
    |-- Channel.{h,cpp} 用于每个 Socket 连接的事件分发
    |-- EventLoop.{h,cpp} 事件分发器
    |-- EventLoopThread.{h,cpp} 新建一个专门用于 EventLoop 的线程
    |-- EventLoopThreadPool.{h,cpp} Muduo 默认多线程 IO 模型
    |-- Poller.{h,cpp} IO multiplexing 的基类接口
    |-- Socket.{h,cpp} 封装 Sockets 描述符，负责关闭连接
    |-- TcpConnection.{h,cpp} muduo 里最大的一个类，有 300 多行
    |-- TcpServer.{h,cpp} TCP 服务端
    |-- Timer.{h,cpp} 以下几个文件与定时器回调相关
    \-- TimerQueue.{h,cpp}
    
\-- tests 为每个类编写的测试用例
    |-- Timestamp_test.cpp
    |-- Thread_test.cpp
    |-- Mutex_test.cpp
    |-- Condition_test.cpp
    |-- CountDownLatch_test.cpp
    |-- BlockingQueue_test.cpp
    |-- ThreadPool_test.cpp
    |-- Timer_test.cpp
    |-- EventLoop_test_1.cpp 
    |-- EventLoop_test_2.cpp 测试基本 EventLoop.poll() Poller.poll() 以及 Channel 关注 fd 事件及回调
    |-- EventLoop_test_3.cpp 测试在 IO 线程中执行其他任务，测试唤醒 poll 阻塞，测试跨线程调用在 IO 线程中执行其他任务
    |-- EventLoop_test_4.cpp 测试 TimerQueue
    |-- Socket_test.cpp
    |-- Acceptor_test.cpp
    |-- Buffer_test.cpp
    |-- TcpConnection_test.cpp
    |-- TcpServer_single_io_test 单 IO 线程服务器测试
    |-- TcpServer_test.cpp 多 IO 线程服务器测试
    |-- TcpServer_test_2.cpp 测试服务器关闭写连接
    \-- TcpServer_test_3 chargen 服务器

\-- samples
    \-- fibonacciServer.cpp 一个 One loop per thread + threadpool 服务器实例，用递归斐波那契数列模拟计算密集型任务，测试并发
``` 

# 笔记（updating）
* [01 Base：Timestamp 及 Thread](https://github.com/SungYK/muduo-lite/blob/master/notes/01.md)
* [02 Base：进程间通信 IPC](https://github.com/SungYK/muduo-lite/blob/master/notes/02.md)
