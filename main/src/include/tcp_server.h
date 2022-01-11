//
// Created by charles.zuo on 2021/12/16.
//

#ifndef ZXCHTTPSERVER_TCP_SERVER_H
#define ZXCHTTPSERVER_TCP_SERVER_H

#include "event_loop.h"
#include "thread_pool.h"

#define LISTEN_QUEUE_SIZE 1024

typedef struct {
    int listenFd;
    ThreadPool *threadPool;
}TcpServer;

typedef struct {
    int connectFd;
    struct EventLoop *eventLoop;
}TcpConnection;

void tcp_server_make_socket_nonblock(int fd);

void tcp_server_make_socket_reuse(int fd);

int tcp_server_init(int port);

int tcp_server_on_tcp_connect(void *data);

#endif //ZXCHTTPSERVER_TCP_SERVER_H
