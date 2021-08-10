#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <unistd.h>
#include <sys/syscall.h>

/* 引入此头文件用于缓存当前线程 tid，减少系统调用次数 */
namespace CurrentThread {
    extern __thread int t_cachedTid;
    inline void cacheTid() {
        t_cachedTid = static_cast<int>(::syscall(SYS_gettid));
    }
    
    inline int tid() {
        if(t_cachedTid == 0) {
            cacheTid();
        }
        return t_cachedTid;
    }
}

#endif