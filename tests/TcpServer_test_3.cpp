/* chargen 服务器
    不断产生数据，然后发送 conn->send(); 如果对方接收不及时，内核缓冲区不足，
    将未发送数据添加到发送缓冲区 output buffer，可能会撑爆 buffer
    解决方式是，调整发送频率，关注 WriteCompleteCallback
    所有数据都发送完，WriteCompleteCallback 回调，然后再继续发送
*/

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

        /* 生成数据 */
        string line;
        for (int i = 33; i < 127; ++i) {
            line.push_back(char(i));
        }
        line += line;

        for (size_t i = 0; i < 127-33; ++i) {
            message_ += line.substr(i, 72) + '\n';
        }

    }

    void onConnection(TcpConnection* conn) {
        if(conn->connected()) {
            cout << "Connection established in thread:" << CurrentThread::tid() << endl;
            
            conn->send(message_);
            // conn->shutDown();
        }
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

    void onWriteCompelete(TcpConnection* conn) {
        conn->send(message_);
    }


    void start() {
        server_.start();
        loop_->loop();
    }

private:
    EventLoop* loop_;
    TcpServer server_;
    int numThreads_;
    string message_;
};


int main()
{

    Server server(4);
    server.start();

    return 0;
}