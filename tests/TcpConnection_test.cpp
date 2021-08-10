#include "src/net/TcpConnection.h"
#include "src/net/Acceptor.h"
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

using namespace std;

EventLoop* g_loop;

void connCallback(TcpConnection* conn) {
    if(conn->connected())
        cout << "Connection established!" << endl;
    else
        cout << "Connection destroyed!" << endl;
}

void closeCallback(TcpConnection* conn) {
    cout << "Client closed connection!" << endl;
}

void writeCompleteCallback(const TcpConnection* conn) {
    cout << "send success msg" << endl;
}

void messageCallback(TcpConnection* conn, Buffer* buf) {
    cout << "recieved: " << buf->retrieveAllAsString();
    
    /* 回写收到 */
    string msg = "Data recieved\n";
    conn->send(msg);

    /* FIXME: tset destroy 存在 bug: destroyed 后无法接收新的连接*/
    // g_loop->runInLoop(bind(&TcpConnection::connectDestroyed, conn));
}

/* 当有新连接时调用，创建 TcpConnection 对象 */
void newConnectionCallback(int fd, sockaddr_in* peeraddr) {

    /* 创建一个 TcpConnection 维护这个连接 */

    TcpConnection* conn(new TcpConnection(g_loop,fd, peeraddr));
    
    conn->setConnectionCallback(connCallback);
    conn->setWriteCompleteCallback(writeCompleteCallback);
    conn->setCloseCallback(closeCallback);
    conn->setMessageCallback(messageCallback);

    g_loop->runInLoop(bind(&TcpConnection::connectEstablished, conn));

    // conn->connectDestroyed();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    Acceptor acceptor(&loop, 9995); // 创建监听套接字
    acceptor.setNewConnectionCallback(newConnectionCallback); // 设置新连接回调
    acceptor.listen(); // 关注 POLLIN 事件，并把监听套接字的 channel 加入 poller 中
    loop.loop();

}