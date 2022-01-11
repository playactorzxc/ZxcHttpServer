//
// Created by charles.zuo on 2021/12/14.
//

#ifndef ZXCHTTPSERVER_EVENT_DISPATCHER_H
#define ZXCHTTPSERVER_EVENT_DISPATCHER_H

#include "channel.h"
#include <sys/epoll.h>
#include "event_loop.h"

const int MAXEVENTS = 128;
typedef struct {
//    int eventCount;
//    int nfds;
//    int reAllocCopy;
    int efd;
    struct epoll_event *events;
} EventDispatcherData;

typedef struct {
    const char *name;

    EventDispatcherData *(*init)();

    int (*add)(struct EventLoop *eventLoop, struct Channel *channel);

    int (*del)(struct EventLoop *eventLoop, struct Channel *channel);

    int (*update)(struct EventLoop *eventLoop, struct Channel *channel);

    int (*dispatch)(struct EventLoop *eventLoop, int timeout);

    void (*clear)(struct EventLoop *eventLoop);
} EventDispatcher;
#endif //ZXCHTTPSERVER_EVENT_DISPATCHER_H
