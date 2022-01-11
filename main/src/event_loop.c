//
// Created by charles.zuo on 2021/12/14.
//

#include <pthread.h>
#include <assert.h>
#include <sys/socket.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/event_loop.h"
#include "include/log.h"

int onWakeup(void *data) {
    // main loop
    struct EventLoop *pEventLoop = (struct EventLoop *) data;
    char buf[1024];
    ssize_t result = read(pEventLoop->socketPair[1], buf, sizeof(buf));
    logd("wakeup read: %s, len: %d", buf, result);
}

struct EventLoop *event_loop_init_main() {
    event_loop_init_by_name("MainThread");
}

struct EventLoop *event_loop_init_by_name(char *name) {
    assert(name != NULL);
    struct EventLoop *eventLoop = malloc(sizeof(struct EventLoop));
    pthread_mutex_init(&eventLoop->mutex, NULL);
    pthread_cond_init(&eventLoop->cond, NULL);
    eventLoop->ownerThreadId = pthread_self();
    eventLoop->threadName = name;

    eventLoop->quit = 0;
    eventLoop->eventDispatcher = &eventDispatcher;
    // No1. 调用epoll_create1创建epoll，并为初始化epoll_event数组
    eventLoop->eventDispatcherData = eventLoop->eventDispatcher->init();
    // 通过socketpair可以唤醒阻塞在sub-reactor线程上的epoll_wait调用
    if (socketpair(PF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair) < 0) {
        error(1, errno, "socket pair failed");
    }
    eventLoop->channelMap = malloc(sizeof(ChannelMap));
    channel_map_init(eventLoop->channelMap);

    // 创建一个Channel,并把soketPair的READ事件和回调绑定到该channel上,等待另一个socketPair通过write调用唤醒
    struct Channel *channel = channel_init(eventLoop->socketPair[1], EVENT_READ, onWakeup, NULL, eventLoop);
    // No2. 调用epoll_ctl注册待处理IO事件
    event_loop_add_channel_event(eventLoop, eventLoop->socketPair[1], channel);

    return eventLoop;
}

int event_loop_run(struct EventLoop *eventLoop) {
    assert(eventLoop != NULL);
    if (eventLoop->ownerThreadId != pthread_self()) {
        exit(1);
    }
    logd("event loop run: %s", eventLoop->threadName);
    const EventDispatcher *dispatcher = eventLoop->eventDispatcher;
    while (!eventLoop->quit) {
        // 阻塞在epoll_wait调用上，循环派发IO事件
        dispatcher->dispatch(eventLoop, 0);

        // 可能是被socketpair唤醒的，处理队列里的事件
        event_loop_consume_channel_event(eventLoop);
    }
}

void event_loop_consume_channel_event(struct EventLoop *eventLoop) {
    pthread_mutex_lock(&eventLoop->mutex);
    eventLoop->isHandlePending = 1;

    ChannelElement *head = eventLoop->channelElementHead;
    //循环处理epoll_ctl事件
    while (head != NULL) {
        struct Channel *channel = head->channel;
        if (head->type == ADD) {
            event_loop_real_add_channel_event(eventLoop, channel);
        } else if (head->type == DELETE) {
            event_loop_real_delete_channel_event(eventLoop, channel);
        } else if (head->type == UPDATE) {
            event_loop_real_update_channel_event(eventLoop, channel);
        }

        head = head->next;
    }
    eventLoop->channelElementHead = eventLoop->channelElementTail = NULL;
    eventLoop->isHandlePending = 0;

    pthread_mutex_unlock(&eventLoop->mutex);
}

void event_loop_real_update_channel_event(struct EventLoop *eventLoop, struct Channel *channel) {
    ChannelMap *map = eventLoop->channelMap;
    if (channel->fd < 0 || map->entries[channel->fd] == NULL) return;
    eventLoop->eventDispatcher->update(eventLoop, channel);
}

void event_loop_real_delete_channel_event(struct EventLoop *eventLoop, struct Channel *channel) {
    ChannelMap *map = eventLoop->channelMap;
    if (channel->fd < 0 || channel->fd >= map->number) return;
    eventLoop->eventDispatcher->del(eventLoop, channel);
    map->entries[channel->fd] = NULL;
}

void event_loop_real_add_channel_event(struct EventLoop *eventLoop, struct Channel *channel) {
    ChannelMap *map = eventLoop->channelMap;
    if (channel->fd < 0) return;
    if (channel->fd > map->number) {
        int makeSpaceResult = channel_map_make_space(map, channel->fd, sizeof(struct Channel *));
        if (makeSpaceResult == -1) return;
    }

    if (map->entries[channel->fd] == NULL) {
        map->entries[channel->fd] = channel;
        eventLoop->eventDispatcher->add(eventLoop, channel);
    }
}

void event_loop_channel_event_activate(struct EventLoop *eventLoop, int fd, int event) {
    ChannelMap *map = eventLoop->channelMap;
    logd("activate event:%d", event);

    if (fd < 0 || fd >= map->number) return;
    struct Channel *channel = map->entries[fd];
    assert(channel->fd == fd);

    if ((event & EVENT_READ) && channel->eventReadCallback) {
        channel->eventReadCallback(channel->data);
    }
    if ((event & EVENT_WRITE) && channel->eventWriteCallback) {
        channel->eventWriteCallback(channel->data);
    }
}


void event_loop_add_channel_event(struct EventLoop *eventLoop, int fd, struct Channel *channel) {
    event_loop_ctl_channel_event(eventLoop, ADD, channel);
}

void event_loop_delete_channel_event(struct EventLoop *eventLoop, int fd, struct Channel *channel) {
    event_loop_ctl_channel_event(eventLoop, DELETE, channel);
}

void event_loop_update_channel_event(struct EventLoop *eventLoop, int fd, struct Channel *channel) {
    event_loop_ctl_channel_event(eventLoop, UPDATE, channel);
}

void event_loop_ctl_channel_event(struct EventLoop *eventLoop, int type, struct Channel *channel) {
    pthread_mutex_lock(&eventLoop->mutex);
    assert(eventLoop->isHandlePending == 0);
    // 先把事件存储到eventLoop得队列里
    event_loop_enqueue_channel(eventLoop, channel, type);
    pthread_mutex_unlock(&eventLoop->mutex);

    if (eventLoop->ownerThreadId != pthread_self()) {
        // 通过socketpair唤醒eventLoop上得线程
        event_loop_wakeup(eventLoop);
    } else {
        // 消费队列里得事件
        event_loop_consume_channel_event(eventLoop);
    }
}

void event_loop_wakeup(struct EventLoop *eventLoop) {
    char msg[128];
    sprintf(msg, "wakeup thread: %s", eventLoop->threadName);
    write(eventLoop->socketPair[0], msg, sizeof(msg));
    logd("send wakeup msg: %s", eventLoop->threadName);
}

void event_loop_enqueue_channel(struct EventLoop *eventLoop, struct Channel *channel, int type) {
    ChannelElement *channelElement = malloc(sizeof(ChannelElement));
    channelElement->type = type;
    channelElement->channel = channel;
    channelElement->next = NULL;

    if (eventLoop->channelElementHead == NULL) {
        eventLoop->channelElementHead = eventLoop->channelElementTail = channelElement;
    } else {
        eventLoop->channelElementTail->next = channelElement;
        eventLoop->channelElementTail = channelElement;
    }
}

