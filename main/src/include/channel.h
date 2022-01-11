//
// Created by charles.zuo on 2021/12/14.
//

#ifndef ZXCHTTPSERVER_CHANNEL_H
#define ZXCHTTPSERVER_CHANNEL_H
#include "event_loop.h"
#define EVENT_TIMEOUT    0x01
/** Wait for a socket or FD to become readable */
#define EVENT_READ        0x02
/** Wait for a socket or FD to become writeable */
#define EVENT_WRITE    0x04
/** Wait for a POSIX signal to be raised*/
#define EVENT_SIGNAL    0x08

typedef int (*event_read_callback)(void *data);
typedef int (*event_write_callback)(void *data);

struct Channel{
    int fd;
    int events;

    event_read_callback eventReadCallback;
    event_write_callback eventWriteCallback;
    void *data;
};

struct Channel * channel_init(int fd,
                              int events,
                              event_read_callback eventReadCallback,
                              event_write_callback eventWriteCallback,
                              void *data);

int channel_write_enable(struct Channel *channel);

void channel_set_write_enable(struct  EventLoop *eventLoop, struct Channel *channel);

void channel_set_write_disable(struct  EventLoop *eventLoop, struct Channel *channel);

#endif //ZXCHTTPSERVER_CHANNEL_H
