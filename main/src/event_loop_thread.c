//
// Created by charles.zuo on 2021/12/14.
//

#include <pthread.h>
#include <assert.h>
#include "include/event_loop_thread.h"

void *runEventLoopThread(void *data) {
    EventLoopThread *eventLoopThread = (EventLoopThread *) data;
    // 子线程执行, 初始化eventLoop, 然后唤醒主线程
    assert(pthread_mutex_lock(&eventLoopThread->mutex) == 0);
    eventLoopThread->eventLoop = event_loop_init_by_name(eventLoopThread->threadName);
    pthread_cond_signal(&eventLoopThread->cond);
    assert(pthread_mutex_unlock(&eventLoopThread->mutex) == 0);

    event_loop_run(eventLoopThread->eventLoop);
}

int event_loop_thread_init(EventLoopThread *eventLoopThread, char *threadName) {
    eventLoopThread->threadName = threadName;
    pthread_mutex_init(&eventLoopThread->mutex, NULL);
    pthread_cond_init(&eventLoopThread->cond, NULL);
    eventLoopThread->eventLoop = NULL;
    eventLoopThread->threadId = 0;
}

struct EventLoop *event_loop_thread_start(EventLoopThread *eventLoopThread) {
    pthread_create(&eventLoopThread->threadId, NULL, runEventLoopThread, eventLoopThread);

    // 主线程继续执行,wait eventLoop 初始化完成
    assert(pthread_mutex_lock(&eventLoopThread->mutex) == 0);
    while (eventLoopThread->eventLoop == NULL) {
        assert(pthread_cond_wait(&eventLoopThread->cond, &eventLoopThread->mutex) == 0);
    }

    assert(pthread_mutex_unlock(&eventLoopThread->mutex) == 0);

    return eventLoopThread->eventLoop;
}