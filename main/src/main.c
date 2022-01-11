//
// Created by charles.zuo on 2021/12/14.
//
#include <malloc.h>
#include "include/event_loop.h"
#include "include/tcp_server.h"
#include "include/thread_pool.h"
#define PORT 35555

int main(int c, char **s) {
    // 创建主线程EventLoop并初始化
    struct EventLoop *eventLoop = event_loop_init_main();

    // tcp socket初始化
    int listenFd = tcp_server_init(PORT);
    // 创建channel ，通过 epoll_ctl 监听listenFd
    TcpServer *tcpServer = malloc(sizeof(TcpServer));
    tcpServer->listenFd = listenFd;

    ThreadPool *threadPool = thread_pool_init(eventLoop, 4);
    thread_pool_start(threadPool);
    tcpServer->threadPool = threadPool;

    // 注册 listenFd 的READ事件的回调函数 tcp_server_on_tcp_connect
    struct Channel *channel = channel_init(listenFd, EVENT_READ, tcp_server_on_tcp_connect, NULL, tcpServer);
    event_loop_add_channel_event(eventLoop, listenFd, channel);

    // main thread for acceptor
    event_loop_run(eventLoop);
}