/* 测试基本 EventLoop.poll() Poller.poll() 以及 Channel 关注 fd 事件及回调 */
#include <src/net/Channel.h>
#include <src/net/EventLoop.h>

#include <sys/timerfd.h>
#include <strings.h>
#include <unistd.h>
#include <iostream>

void timeout(int fd, EventLoop* loop) {
    cout << "timeout!" << endl;
    /* 处理POLLIN事件，避免 busy loop */
    uint64_t buf;
    ssize_t n = read(fd, &buf, sizeof(buf));
    if(n<0){}
    loop->quit();
}

int main()
{
    EventLoop loop;

    /* 创建 timerfd */
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    /* 创建 channel，设置读回调，并关注读事件 */
    Channel channel(&loop, timerfd);
    channel.setReadCallback(bind(timeout, timerfd, &loop));
	channel.enableReading();

    /* 设置 timerfd 超时时间 */
    struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1; // 1秒
	timerfd_settime(timerfd, 0, &howlong, NULL);


    loop.loop();
    close(timerfd);

    return 0;
}