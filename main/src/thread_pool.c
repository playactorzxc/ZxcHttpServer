//
// Created by charles.zuo on 2021/12/14.
//
#include <malloc.h>
#include <assert.h>
#include "include/thread_pool.h"

ThreadPool *thread_pool_init(struct EventLoop *eventLoop, int threadNum) {
    ThreadPool *threadPool = malloc(sizeof(ThreadPool));
    threadPool->mainLoop = eventLoop;
    threadPool->threadNum = threadNum;
    threadPool->eventLoopThread = NULL;
    threadPool->position = 0;
    threadPool->started = 0;
    return threadPool;
}

void thread_pool_start(ThreadPool *threadPool) {
    assert(!threadPool->started);
    threadPool->started = 1;

    if (threadPool->threadNum <= 0) return;
    
    threadPool->eventLoopThread = calloc(sizeof(EventLoopThread), threadPool->threadNum);
    for (int i = 0; i < threadPool->threadNum; ++i) {
        char *threadName = malloc(128);
        sprintf(threadName, "ThreadPoll-%d", i);
        event_loop_thread_init(&threadPool->eventLoopThread[i], threadName);
        event_loop_thread_start(&threadPool->eventLoopThread[i]);
    }
}

struct EventLoop *thread_pool_get_loop(ThreadPool *threadPool) {
    assert(threadPool->started);
    // 必须在主线程调用
    struct EventLoop *selectLoop = threadPool->mainLoop;
    assert(selectLoop->ownerThreadId == pthread_self());
    
    if(threadPool->threadNum > 0) {
        selectLoop = threadPool->eventLoopThread[threadPool->position].eventLoop;
        if (++threadPool->position >= threadPool->threadNum) {
            threadPool->position = 0;
        }
    }

    return selectLoop;
}