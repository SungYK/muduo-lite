#include "src/net/EventLoop.h"
#include "src/net/Acceptor.h"
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

void newConnectionCallback(int fd, sockaddr_in* peeraddr) {
  printf("newConnection(): accepted a new connection from %d\n",
         peeraddr->sin_port);
  ssize_t ret = write(fd, "How are you?\n", 13);
  if(ret < 0) {}
  close(fd);
}

int main()
{
    EventLoop loop;
    Acceptor acceptor(&loop, 9995);
    acceptor.setNewConnectionCallback(newConnectionCallback);
    acceptor.listen();
    loop.loop();
    return 0;
}

/* $ nc 127.0.0.1 9995 */