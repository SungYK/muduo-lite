#include "src/net/Socket.h"
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>

int Socket::createFd() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | 
                    SOCK_CLOEXEC, 0);

    if(sockfd < 0) {
        perror("create fd error");
    }

    return sockfd;
}

Socket::Socket(int sockfd) 
    : sockfd_(sockfd) { 
    setReusePort(true);
    setKeepAlive(true);
}

Socket::~Socket() {
    close(sockfd_);
}

int Socket::fd() {
    return sockfd_;
}

void Socket::bindAddress(int bindPort) {
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(bindPort);
    int ret = bind(sockfd_, reinterpret_cast<struct sockaddr*>(&servaddr),
                     sizeof(servaddr));
    if(ret == -1) {
        perror("bind error");
    }
}
 
void Socket::listen() {
    int ret = ::listen(sockfd_, SOMAXCONN);
    if(ret < 0) {
        perror("listen error");
    }
}

int Socket::accept(struct sockaddr_in *peeraddr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
    int connfd = accept4(sockfd_, reinterpret_cast<struct sockaddr*>(&peeraddr), 
                        &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd < 0) {
        perror("connfd error");
    }
    return connfd;
}

void Socket::shutdownWrite() {

    if(shutdown(sockfd_, SHUT_WR) < 0) {
        perror("shutdown write error");
    }
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                        &optval, static_cast<socklen_t>(sizeof optval));
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
            &optval, static_cast<socklen_t>(sizeof optval));
}