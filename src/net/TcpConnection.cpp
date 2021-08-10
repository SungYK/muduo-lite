#include "src/net/TcpConnection.h"
#include <unistd.h>
#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop, int sockfd, sockaddr_in* peeraddr) 
    : loop_(loop),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    peeraddr_(peeraddr),
    state_(kConnecting),
    id_(-1) {
    
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {}

void TcpConnection::setConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
}
void TcpConnection::setCloseCallback(const CloseCallback& cb) {
    closeCallback_ = cb;
}
void TcpConnection::setWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
}
void TcpConnection::setMessageCallback(const MessageCallback& cb) {
    messageCallback_ = cb;
}

void TcpConnection::send(const std::string& message) {
    if(state_ == kConnected) {
        if(loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

// TODO: meassage 改为 void* 类型
void TcpConnection::sendInLoop(const std::string& message) {
    ssize_t n = 0;
    if(state_ == kDisconnected) {
        return;
    }

    /* 如果 channel 没有在处理写事件，并且缓冲区中不存在未发送完的数据，则直接发送 */
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        n = write(channel_->fd(), message.c_str(), message.size());
        if(n < 0)
            perror("sendInLoop write error");

        if(n == static_cast<ssize_t>(message.size()) && writeCompleteCallback_) {
            loop_->queueInLoop(std::bind(writeCompleteCallback_, this));
        }
    }

    /* 数据没有发送完，内核缓冲区满，将数据存入 outputBuffer_ 缓冲区，
    并关注POLLOUT事件，在内核缓冲区空出时调用 handleWrite 发送 */
    if(n < static_cast<ssize_t>(message.size())) {
        outputBuffer_.append(message.substr(n, message.size()));
        if(!channel_->isWriting()) {
            channel_->enableWriting(); // 关注 POLLOUT 事件
        }
    }
}

void TcpConnection::shutDown() {
    if(state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutDownInLoop, this));
    }
}

void TcpConnection::shutDownInLoop() {
    if(!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void TcpConnection::handleRead() {

    int sockfd = channel_->fd();
    ssize_t nRead = 0;
    char buf[BUFSIZ] = {0};
    
    nRead = read(sockfd, buf, BUFSIZ);
    if(nRead > 0) {
        inputBuffer_.append(string(buf, nRead));
        messageCallback_(this, &inputBuffer_);
    } else if(nRead == 0) {
        handleClose();
    } else {
        perror("TcpConnection::handleread error");
        handleError();
    }
}

/* 处理 POLLOUT 事件，继续发送 outbuf 缓冲区中未发送完的数据 */
void TcpConnection::handleWrite() {
    if(channel_->isWriting()) {
        ssize_t n = write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());

        if(n > 0) {
            outputBuffer_.retrieve(static_cast<int>(n)); // 从 outbuf 中取出已写部分
            if(outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting(); // 数据写完发送完成，取消关注 POLLOUT 事件
                if(writeCompleteCallback_) {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, this));
                }
                /* 若之前有尝试关闭，有可能之前 channel 处于 writing 状态，
                没有关闭成功，这里再次关闭 */
                if(state_ == kDisconnecting) {  
                    shutDownInLoop();
                }
            }

        } else {
            perror("TcpConnection::handleWrite n <= 0");
        }
    } else {
        std::cout << "channel has no event" << std::endl;
    }
}

void TcpConnection::handleClose() {
    channel_->disableAll();
    setState(kDisconnected);
    connectionCallback_(this);

    // FIXME: 
    // if(closeCallback_)
    //     closeCallback_(this);
}

/* TODO: show socket error */
void TcpConnection::handleError() {
    std::cout << "TcpConnection::handleError some error happend" << std::endl;
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(this);
}


void TcpConnection::connectDestroyed() {

    if(state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(this);
    }
    channel_->remove();
}

void TcpConnection::setState(StateE s) {
    state_ = s;
}

void TcpConnection::setId(int id) {
    if(id_ == -1)
        id_ = id;
}

int TcpConnection::id() {
    return id_;
}

bool TcpConnection::connected() {
    return state_ == kConnected; 
}

EventLoop* TcpConnection::getLoop() {
    return loop_;
}