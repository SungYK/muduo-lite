/* 测试服务器关闭写连接 */

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
        if(conn->connected()) {
            cout << "Connection established in thread:" << CurrentThread::tid() << endl;
            string msg = "Connected!\n";
            conn->send(msg);
            // conn->shutDown();
        }
        else
            cout << "Connection destroyed! in thread:" << CurrentThread::tid() << endl;
    }

    void onMessage(TcpConnection* conn, Buffer* buf) {
        cout << "handle compute task in thread: " << CurrentThread::tid() << 
            " recieved: " << buf->retrieveAllAsString();
        
        /* 回写收到，已经关闭写连接，客户端不会收到消息 */
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

    Server server(4);
    server.start();

    return 0;
}