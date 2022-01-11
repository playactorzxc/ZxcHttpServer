//
// Created by charles.zuo on 2021/12/14.
//

#ifndef ZXCHTTPSERVER_THREAD_POOL_H
#define ZXCHTTPSERVER_THREAD_POOL_H

#include "event_loop.h"
#include "event_loop_thread.h"
#include <pthread.h>

typedef struct {
    //创建线程池的主线程
    struct EventLoop *mainLoop;
    //标识线程池里所有线程已经被start
    int started;
    //线程总数=线程池数组长度
    int threadNum;
    //线程池数组首地址
    EventLoopThread *eventLoopThread;
    //指向下一个被选中的线程在数组中的下标
    int position;
}ThreadPool;

ThreadPool *thread_pool_init(struct EventLoop *eventLoop, int threadNum);

void thread_pool_start(ThreadPool *threadPool);

struct EventLoop *thread_pool_get_loop(ThreadPool *threadPool);

#endif //ZXCHTTPSERVER_THREAD_POOL_H
