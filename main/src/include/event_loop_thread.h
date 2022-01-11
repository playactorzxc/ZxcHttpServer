//
// Created by charles.zuo on 2021/12/14.
//

#ifndef ZXCHTTPSERVER_EVENT_LOOP_THREAD_H
#define ZXCHTTPSERVER_EVENT_LOOP_THREAD_H
#include "event_loop.h"
typedef struct {
    struct EventLoop *eventLoop;
    pthread_t threadId;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char *threadName;
} EventLoopThread;

int event_loop_thread_init(EventLoopThread *eventLoopThread, char *threadName);

struct EventLoop *event_loop_thread_start(EventLoopThread *eventLoopThread);

#endif //ZXCHTTPSERVER_EVENT_LOOP_THREAD_H
