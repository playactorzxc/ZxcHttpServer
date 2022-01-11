//
// Created by charles.zuo on 2021/12/20.
//

#ifndef ZXCHTTPSERVER_BUFFER_H
#define ZXCHTTPSERVER_BUFFER_H

struct buffer{
    char *data;
    int readIndex;
    int writeIndex;
    int totalSize;
};

struct buffer *buffer_new();

void buffer_free(struct buffer *buffer);

int buffer_writeable_size(struct buffer *buffer);

int buffer_readable_size(struct buffer *buffer);

int buffer_append(struct buffer *buffer, void *data, int size);

int buffer_append_char(struct buffer *buffer, char data);

int buffer_append_string(struct buffer *buffer, char* data);

int buffer_read_socket(struct buffer *buffer, int fd);



#endif //ZXCHTTPSERVER_BUFFER_H
