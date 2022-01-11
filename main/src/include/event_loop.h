//
// Created by charles.zuo on 2021/12/14.
//

#ifndef ZXCHTTPSERVER_EVENT_LOOP_H
#define ZXCHTTPSERVER_EVENT_LOOP_H

#include "event_dispatcher.h"
#include "channel_map.h"

#define ADD  1
#define DELETE  2
#define UPDATE  3

extern const EventDispatcher eventDispatcher;

typedef struct channel_element ChannelElement;

struct channel_element {
    int type; // add / delete / update

    struct Channel *channel;
    ChannelElement *next;
};

struct EventLoop {
    int quit;
    const EventDispatcher *eventDispatcher;
    EventDispatcherData *eventDispatcherData;

    char *threadName;
    pthread_t ownerThreadId;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int socketPair[2];
    ChannelMap *channelMap;

    int isHandlePending;
    // 待处理channel链表
    ChannelElement *channelElementHead;
    ChannelElement *channelElementTail;
};


struct EventLoop *event_loop_init_main();

struct EventLoop *event_loop_init_by_name(char *name);

int event_loop_run(struct EventLoop *eventLoop);

void event_loop_channel_event_activate(struct EventLoop *eventLoop, int fd, int event);

void event_loop_ctl_channel_event(struct EventLoop *eventLoop, int type, struct Channel *channel);

void event_loop_add_channel_event(struct EventLoop *eventLoop, int fd, struct Channel *channel);

void event_loop_delete_channel_event(struct EventLoop *eventLoop, int fd, struct Channel *channel);

void event_loop_update_channel_event(struct EventLoop *eventLoop, int fd, struct Channel *channel);

void event_loop_real_add_channel_event(struct EventLoop *eventLoop, struct Channel *channel);

void event_loop_real_delete_channel_event(struct EventLoop *eventLoop, struct Channel *channel);

void event_loop_real_update_channel_event(struct EventLoop *eventLoop, struct Channel *channel);

void event_loop_enqueue_channel(struct EventLoop *eventLoop, struct Channel *channel, int type);

void event_loop_consume_channel_event(struct EventLoop *eventLoop);

void event_loop_wakeup(struct EventLoop *eventLoop);

#endif //ZXCHTTPSERVER_EVENT_LOOP_H
