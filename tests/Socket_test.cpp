/* 使用一个简单的 epoll server 测试 Socket */
#include "src/net/Socket.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>


using namespace std;

int main()
{
    /* Socket 封装的 bind() listen() */ 
    Socket socket(Socket::createFd());
    socket.bindAddress(9995);
    socket.listen();

    int epollfd = epoll_create1(EPOLL_CLOEXEC);
    struct epoll_event event;
    int listenfd = socket.fd();
    event.data.fd = listenfd;
    event.events = EPOLLIN;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);

    vector<struct epoll_event> events(16);

    int nready;
    struct sockaddr_in peeraddr;
    while(1) {
        // c++ 11 可用 pollfds.data() 取首地址
        nready = epoll_wait(epollfd, events.data(), static_cast<int>(events.size()), -1);
        if(nready == -1) {
            perror("epoll wait");
        }
        if(nready == 0) {
            continue;
        }

        if(static_cast<size_t>(nready) == events.size())
            events.resize(events.size()*2);

        for(int i=0; i<nready; i++) {
            int connfd;
            if(events[i].data.fd == listenfd) {

                connfd = socket.accept(&peeraddr); // Socket 封装的 baccept()     
 
                event.data.fd = connfd;
                event.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event);
                nready--;

                cout << "ip=" << inet_ntoa(peeraddr.sin_addr) << 
                        "port=" << ntohs(peeraddr.sin_port) << endl;
                if(nready == 0)
                    continue;
            }

            else if(events[i].events & EPOLLIN) {
                connfd = events[i].data.fd;

                if(connfd < 0)
                    continue;
                char buf[1024] = {0};
                ssize_t ret = read(connfd, buf, sizeof(buf));
                if(ret == -1)
                    perror("read");
                if(ret == 0) {
                    cout << "client closed" << endl;
                    close(connfd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, NULL);
                    continue;
                }
                cout << buf;
                ret = write(connfd, buf, strlen(buf));
            }
        }
    }

    return 0;
}