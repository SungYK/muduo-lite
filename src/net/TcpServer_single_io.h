#ifndef TCPSERVER_SINGLE_IO_H
#define TCPSERVER_SINGLE_IO_H

#include "src/net/TcpConnection.h"
#include "src/net/Acceptor.h"

#include <map>

/* 单 IO 线程（一个 EventLoop） TcpServer */
class TcpServer {
public:
    typedef std::function<void (TcpConnection*)> ConnectionCallback;
    typedef std::function<void (TcpConnection*)> WriteCompleteCallback;
    typedef std::function<void (TcpConnection*, Buffer*)> MessageCallback;

    TcpServer(EventLoop* loop, int bindPort);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb);
    void setWriteCompleteCallback(const WriteCompleteCallback& cb);
    void setMessageCallback(const MessageCallback& cb);

private:

    void newConnection(int sockfd, sockaddr_in* peeraddr);
    void removeConnection(TcpConnection* conn);
    void removeConnectionInLoop(TcpConnection* conn);

    EventLoop* loop_;
    Acceptor* acceptor_;
    ConnectionCallback connectionCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    MessageCallback messageCallback_;
    std::map<unsigned int, TcpConnection*> connections_;
    bool started_;
    unsigned int connectionIndex_;
};

#endif