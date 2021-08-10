#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "src/net/EventLoop.h"
#include "src/net/Socket.h"


class Acceptor {
public:
    typedef std::function<void (int sockfd, sockaddr_in* peeraddr)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const int bindPort);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb);

    void listen();

    bool listening();

private:
    void handleRead(); // 处理连接事件

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idlefd_; // 
};

#endif