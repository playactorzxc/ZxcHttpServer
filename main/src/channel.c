//
// Created by charles.zuo on 2021/12/14.
//
#include <malloc.h>
#include "include/channel.h"

struct Channel *channel_init(int fd,
                             int events,
                             event_read_callback eventReadCallback,
                             event_write_callback eventWriteCallback,
                             void *data) {
    struct Channel *channel = malloc(sizeof(struct Channel));
    channel->fd = fd;
    channel->events = events;
    channel->eventReadCallback = eventReadCallback;
    channel->eventWriteCallback = eventWriteCallback;
    channel->data = data;
    return channel;
}

int channel_write_enable(struct Channel *channel) {
    return channel->events & EVENT_WRITE;
}

void channel_set_write_enable(struct EventLoop *eventLoop, struct Channel *channel) {
    channel->events |= EVENT_WRITE;
    event_loop_update_channel_event(eventLoop, channel->fd, channel);
}

void channel_set_write_disable(struct EventLoop *eventLoop, struct Channel *channel) {
    channel->events &= ~EVENT_WRITE;
    event_loop_update_channel_event(eventLoop, channel->fd, channel);
}