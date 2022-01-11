//
// Created by charles.zuo on 2021/12/14.
//
#include <malloc.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include "include/event_dispatcher.h"
#include "include/log.h"

EventDispatcherData *event_dispatcher_init() {
    EventDispatcherData *eventDispatcherData = malloc(sizeof(EventDispatcherData));
    eventDispatcherData->efd = epoll_create1(0);
    if (eventDispatcherData->efd == -1) {
        error(1,errno, "epoll create failed");
    }

    eventDispatcherData->events = calloc(MAXEVENTS, sizeof(struct epoll_event));
}

static int event_dispatcher_ctl(struct EventLoop *eventLoop, int op, struct Channel *channel){
    EventDispatcherData *eventDispatcherData = eventLoop->eventDispatcherData;
    struct epoll_event event;
    event.data.fd = channel->fd;
    event.events = 0;
    if (channel->events & EVENT_READ) {
        event.events |= EPOLLIN;
    }
    if (channel->events & EVENT_WRITE) {
        event.events |= EPOLLOUT;
    }
    if (epoll_ctl(eventDispatcherData->efd, op, channel->fd, &event) == -1) {
        error(1, errno, "epoll_ctl %d fd：%d failed", op, channel->fd);
    }

    return 0;
}

int add(struct EventLoop *eventLoop, struct Channel *channel) {
    event_dispatcher_ctl(eventLoop, EPOLL_CTL_ADD, channel);
}

int del(struct EventLoop *eventLoop, struct Channel *channel) {
    event_dispatcher_ctl(eventLoop, EPOLL_CTL_DEL, channel);
}

int update(struct EventLoop *eventLoop, struct Channel *channel) {
    event_dispatcher_ctl(eventLoop, EPOLL_CTL_MOD, channel);
}



int dispatch(struct EventLoop *eventLoop, int timeout) {
    EventDispatcherData *data = eventLoop->eventDispatcherData;
    int i,n;
    n = epoll_wait(data->efd, data->events, MAXEVENTS, -1);
    for (i = 0; i < n; ++i) {
        if ((data->events[i].events & EPOLLERR)  || (data->events[i].events & EPOLLHUP)) {
            logd("epoll error");
            close(data->events[i].data.fd);
            continue;
        }

        if (data->events[i].events & EPOLLIN) {
            logd("get message channel fd==%d for read, %s", data->events[i].data.fd, eventLoop->threadName);
            event_loop_channel_event_activate(eventLoop, data->events[i].data.fd, EVENT_READ);
        }

        if (data->events[i].events & EPOLLOUT) {
            logd("get message channel fd=%d for write, %s", data->events[i].events.fd, eventLoop->threadName);
            event_loop_channel_event_activate(eventLoop, data->events[i].data.fd, EVENT_WRITE);
        }
    }
}


void clear(struct EventLoop *eventLoop) {

}

const EventDispatcher eventDispatcher = {
        "epoll",
        event_dispatcher_init,
        add,
        del,
        update,
        dispatch,
        clear
};
