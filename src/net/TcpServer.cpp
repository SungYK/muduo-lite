#include "src/net/TcpServer.h"
#include "src/net/EventLoopThreadPool.h"
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, int bindPort) 
    : loop_(loop), 
    acceptor_(new Acceptor(loop, bindPort)),
    threadPool_(new EventLoopThreadPool(loop)),
    started_(false),
    connectionIndex_(0) {
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    for(auto& it : connections_) {
        TcpConnection* conn = it.second;
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn)
        );
        free(conn);
    }
}

void TcpServer::setThreadNum(int numThreads) {
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    
    if(!started_) {
        started_ = true;
        threadPool_->start();
        loop_->runInLoop(
            std::bind(&Acceptor::listen, acceptor_)
        );
    }
}

void TcpServer::newConnection(int sockfd, sockaddr_in* peeraddr) {
    EventLoop *ioLoop = threadPool_->getNextLoop(); // 选择一个 EventLoop 线程

    TcpConnection* conn(new TcpConnection(ioLoop, sockfd, peeraddr));

    conn->setId(connectionIndex_);
    connections_[connectionIndex_] = conn;
    connectionIndex_++;

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, _1)
    );
    ioLoop->runInLoop(
        std::bind(&TcpConnection::connectEstablished, conn)
    );
}

void TcpServer::removeConnection(TcpConnection* conn) {
    loop_->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn)
    );
}

void TcpServer::removeConnectionInLoop(TcpConnection* conn) {
    connections_.erase(conn->id());
    loop_->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn)
    );
}

void TcpServer::setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
}

void TcpServer::setMessageCallback(const MessageCallback& cb) {
    messageCallback_ = cb;
}

void TcpServer::setWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
}