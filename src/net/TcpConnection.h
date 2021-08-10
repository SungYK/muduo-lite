#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "src/net/EventLoop.h"
#include "src/net/Socket.h"
#include "src/net/Buffer.h"

class TcpConnection {
public:

    typedef std::function<void (TcpConnection*)> ConnectionCallback;
    typedef std::function<void (TcpConnection*)> CloseCallback;
    typedef std::function<void (TcpConnection*)> WriteCompleteCallback;
    typedef std::function<void (TcpConnection*, Buffer*)> MessageCallback;

    TcpConnection(EventLoop* loop, int sockfd, sockaddr_in* peeraddr);
    ~TcpConnection();

    void send(const std::string& message);
    void shutDown();
    bool connected();
    // void startRead();
    // void stopRead();

    void setConnectionCallback(const ConnectionCallback& cb);
    void setCloseCallback(const CloseCallback& cb);
    void setWriteCompleteCallback(const WriteCompleteCallback& cb);
    void setMessageCallback(const MessageCallback& cb);

    void connectEstablished();
    void connectDestroyed();

    void setId(int id);
    int id();


    EventLoop* getLoop();

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string& message);
    void shutDownInLoop();
    // void startReadInLoop();
    // void stopReadInLoop();

    EventLoop* loop_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    sockaddr_in* peeraddr_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    MessageCallback messageCallback_;
    void setState(StateE s);
    
    StateE state_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    int id_;

};

#endif