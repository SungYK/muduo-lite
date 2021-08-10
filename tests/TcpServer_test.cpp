#include "src/net/TcpServer.h"
#include "src/net/TcpConnection.h"
#include "src/base/CurrentThread.h"
#include <iostream>

using namespace std;
using namespace std::placeholders;

class Server {
public:

    Server(int numThreads)
    : loop_(new EventLoop),
    server_(loop_, 9995),
    numThreads_(numThreads) {
        server_.setThreadNum(numThreads_);
        server_.setConnectionCallback(bind(&Server::onConnection, this, _1));
        server_.setMessageCallback(bind(&Server::onMessage, this, _1, _2));
    }

    void onConnection(TcpConnection* conn) {
        if(conn->connected())
            cout << "Connection established in thread:" << CurrentThread::tid() << endl;
        else
            cout << "Connection destroyed! in thread:" << CurrentThread::tid() << endl;
    }

    void onMessage(TcpConnection* conn, Buffer* buf) {
        cout << "handle compute task in thread: " << CurrentThread::tid() << 
            " recieved: " << buf->retrieveAllAsString();
        
        /* 回写收到 */
        string msg = "Data recieved\n";
        conn->send(msg);
    }

    void start() {
        server_.start();
        loop_->loop();
    }

private:
    EventLoop* loop_;
    TcpServer server_;
    int numThreads_;
};

int main()
{

    /* 测试多io线程 */
    Server server(4);
    server.start();

    /* 测试单io线程，不开启io线程池 */
    // Server server(0);
    // server.start();

    return 0;
}