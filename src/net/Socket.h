#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

class Socket {
public:
    Socket(int sockfd);
    ~Socket();

    int fd();
    static int createFd();

    /* 用于 Acceptor 的操作，用于监听连接的套接字描述符 */
    void bindAddress(int bindPort);
    void listen();
    int accept(struct sockaddr_in *peeraddr);

    void setReusePort(bool on);
    void setKeepAlive(bool on);

    void shutdownWrite(); // 关闭套接字写连接

private:
    const int sockfd_;
};

#endif