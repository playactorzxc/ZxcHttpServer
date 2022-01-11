//
// Created by charles.zuo on 2021/12/15.
//

#ifndef ZXCHTTPSERVER_CHANNEL_MAP_H
#define ZXCHTTPSERVER_CHANNEL_MAP_H

#include "channel.h"

typedef struct {
    // The number of entries available in entries
    int number;
    // struct Channel * 数组
    struct Channel **entries;
} ChannelMap;

void channel_map_init(ChannelMap *map);

int channel_map_make_space(ChannelMap *map, int slot, int mSize);

void channel_map_clear(ChannelMap *map);

#endif //ZXCHTTPSERVER_CHANNEL_MAP_H
