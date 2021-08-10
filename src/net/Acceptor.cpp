#include "src/net/Acceptor.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

Acceptor::Acceptor(EventLoop* loop, const int bindPort) 
    : loop_(loop), 
    acceptSocket_(Socket::createFd()),
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false),
    idlefd_(open("/dev/null", O_RDONLY | O_CLOEXEC)) {

    acceptSocket_.bindAddress(bindPort);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    close(idlefd_);
}

void Acceptor::listen() {
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading(); // 关注 POLLIN 事件，并把 channel 加入 poller 中
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback& cb) {
    newConnectionCallback_ = cb;
}

void Acceptor::handleRead() {

    struct sockaddr_in peeraddr;
    int connfd = acceptSocket_.accept(&peeraddr);
    if(connfd >=0) {
        if(newConnectionCallback_) {
            newConnectionCallback_(connfd, &peeraddr);
        } else {
            close(connfd);
        }
    } else {
        /* accept EMFILE 返回处理，太多文件描述符
	      准备一个空闲的文件描述符，若文件描述符达到上限，
          则使用这个文件描述与客户端连接，
          并立即关闭文件描述符，之后再打开这个空闲描述符。
          优雅的与客户端断开连接
        */
        if(errno == EMFILE) {
            close(idlefd_);
            idlefd_ = accept(acceptSocket_.fd(), NULL, NULL);
            close(idlefd_);
            idlefd_ = open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
        
    }

}

