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
    |-- CountDownLatch.{h,cc} “倒计时门闩”同步
    |-- Mutex.h 互斥器
    |-- Thread.{h,cc} 线程对象
    \-- ThreadPool.{h,cc} 简单的固定大小线程池

\-- net
    |-- Acceptor.{h,cc} 接受器，用于服务端接受连接
    |-- Buffer.{h,cc} 缓冲区，非阻塞 IO 必备
    |-- Channel.{h,cc} 用于每个 Socket 连接的事件分发
    |-- EventLoop.{h,cc} 事件分发器
    |-- EventLoopThread.{h,cc} 新建一个专门用于 EventLoop 的线程
    |-- EventLoopThreadPool.{h,cc} Muduo 默认多线程 IO 模型
    |-- Poller.{h,cc} IO multiplexing 的基类接口
    |-- Socket.{h,cc} 封装 Sockets 描述符，负责关闭连接
    |-- TcpConnection.{h,cc} muduo 里最大的一个类，有 300 多行
    |-- TcpServer.{h,cc} TCP 服务端
    |-- Timer.{h,cc} 以下几个文件与定时器回调相关
    \-- TimerQueue.{h,cc}
``` 

# 笔记（updating）
* [01 Base：Timestamp 及 Thread](https://github.com/SungYK/muduo-lite/blob/master/notes/01.md)
* [02 Base：进程间通信 IPC](https://github.com/SungYK/muduo-lite/blob/master/notes/02.md)
