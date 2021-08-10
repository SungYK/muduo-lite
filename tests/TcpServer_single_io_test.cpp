#include "src/net/TcpServer_single_io.h"
#include "src/net/TcpConnection.h"
#include <iostream>

using namespace std;
using namespace std::placeholders;

class Server {
public:

    Server()
    : loop_(new EventLoop),
    server_(loop_, 9995) {
        server_.setConnectionCallback(bind(&Server::onConnection, this, _1));
        server_.setMessageCallback(bind(&Server::onMessage, this, _1, _2));
    }

    void onConnection(TcpConnection* conn) {
        if(conn->connected())
            cout << "Connection established!" << endl;
        else
            cout << "Connection destroyed!" << endl;
    }

    void onMessage(TcpConnection* conn, Buffer* buf) {
        cout << "recieved: " << buf->retrieveAllAsString();
        
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
    
};

int main()
{
    Server server;
    server.start();

    return 0;
}