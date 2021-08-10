#include <src/net/Channel.h>
#include <src/net/EventLoop.h>
#include <src/base/CurrentThread.h>
#include <src/base/Thread.h>

void threadFunc()
{
	printf("threadFunc(): pid = %d, tid = %d\n",
		getpid(), CurrentThread::tid());

	EventLoop loop;
	loop.loop();
}

int main(void)
{
	printf("main(): pid = %d, tid = %d\n",
		getpid(), CurrentThread::tid());

	EventLoop loop;
    
	Thread t(threadFunc);
	t.start();

	loop.loop();
	t.join();
	return 0;
}