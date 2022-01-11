//
// Created by charles.zuo on 2021/12/15.
//

#include <stddef.h>
#include <malloc.h>
#include <memory.h>
#include "include/channel_map.h"

void channel_map_init(ChannelMap *map) {
    map->number = 0;
    map->entries = NULL;
}

int channel_map_make_space(ChannelMap *map, int slot, int mSize) {
    if(map->number <= slot) {
        int nentries = map->number > 0 ? map->number : 32;
        while (nentries <= slot)
            nentries <<= 1;
        struct Channel **tmp = realloc(map->entries, nentries * mSize);
        if(tmp == NULL) {
            return -1;
        }

        memset(&tmp[map->number], 0, (nentries - map->number) * mSize);
        map->number = nentries;
        map->entries = tmp;
    }
    return 0;
}

void channel_map_clear(ChannelMap *map) {
    if(map->entries != NULL) {
        for (int i = 0; i < map->number; ++i) {
            if(map->entries[i] != NULL) {
                free(map->entries[i]);
            }
        }
        free(map->entries);
        map->entries = NULL;
    }
    map->number = 0;
}