/* reactor(one loop per thread) + threadpool 
    多io线程，多任务线程，斐波那契数列服务器*/

#include "src/net/TcpServer.h"
#include "src/net/TcpConnection.h"
#include "src/base/CurrentThread.h"
#include "src/base/ThreadPool.h"
#include <iostream>

using namespace std;
using namespace std::placeholders;

class FibServer {
public:

    FibServer(int numIOThreads, int numComptThreads)
    : loop_(new EventLoop),
    server_(loop_, 9995),
    numIOThreads_(numIOThreads),
    numComptThreads_(numComptThreads) {
        server_.setThreadNum(numIOThreads_);
        server_.setConnectionCallback(bind(&FibServer::onConnection, this, _1));
        server_.setMessageCallback(bind(&FibServer::onMessage, this, _1, _2));
    }

    void onConnection(TcpConnection* conn) {
        if(conn->connected()) {
            cout << "Connection established in thread:" << CurrentThread::tid() << endl;
            string msg = "Connected!\n";
            conn->send(msg);
        }
        else
            cout << "Connection destroyed! in thread:" << CurrentThread::tid() << endl; 
    }

    void onMessage(TcpConnection* conn, Buffer* buf) {

        int num = stoi(buf->retrieveAllAsString()); // 没有检查值

        cout << "handle compute task in thread: " << CurrentThread::tid() << 
            " recieved: " << num << endl;
        
        // 将计算任务交到计算线程池中线程，loop 线程继续监听 io 事件
        threadPool_.run(bind(&FibServer::sendFib, this, num, conn));
        
    }


    void start() {
        threadPool_.start(numComptThreads_); // 线程池中创建 numComptThreads_ 个线程用于处理计算任务
        server_.start();
        loop_->loop();
    }

    /* 在子线程中进行计算和再交回 io 线程发送 */
    void sendFib(int n, TcpConnection* conn) {
        int ret = fib(n);
        string msg = "result are " + to_string(ret) + "\n";

        /* 交回对应 loop 线程发送 */
        conn->getLoop()->runInLoop(bind(&TcpConnection::send, conn, msg));
    }

    /* 递归计算Fibonacci数列，计算密集型任务，n 较大时会阻塞当前线程 */
    int fib(int n) {
        if(n <= 2)
            return 1;
        return fib(n-1) + fib(n-2);
    }

private:
    EventLoop* loop_;
    TcpServer server_;
    ThreadPool threadPool_;
    int numIOThreads_;
    int numComptThreads_;
};

int main()
{
    
    FibServer server(2, 8); // 只开启 2 个 loop 线程，初始化 8 个计算线程
    server.start();

    return 0;
}