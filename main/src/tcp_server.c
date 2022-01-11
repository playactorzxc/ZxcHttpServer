//
// Created by charles.zuo on 2021/12/16.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <assert.h>
#include <malloc.h>
#include <unistd.h>
#include "include/tcp_server.h"


int onRead(void *data){
    TcpConnection *tcpConnection = (TcpConnection *) data;
    struct EventLoop *eventLoop = tcpConnection->eventLoop;
    int fd = tcpConnection->connectFd;
    struct Channel *channel = eventLoop->channelMap->entries[fd];

    //todo
    //读完数据后更新epoll事件状态为可写
    channel_set_write_enable(eventLoop, channel);
}

int onWrite(void *data){
    TcpConnection *tcpConnection = (TcpConnection *) data;
    struct EventLoop *eventLoop = tcpConnection->eventLoop;
    int fd = tcpConnection->connectFd;
    struct Channel *channel = eventLoop->channelMap->entries[fd];
    //todo
    //写完数据后更新epoll事件状态为不可写
    channel_set_write_disable(eventLoop, channel);
}

int tcp_server_init(int port) {
    int listenFd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_server_make_socket_nonblock(listenFd);

    tcp_server_make_socket_reuse(listenFd);

    int bindResult = bind(listenFd, (const struct sockaddr *) &addr, sizeof(addr));
    if (bindResult < 0) {
        error(1, errno, "bind failed");
    }

    int listenResult = listen(listenFd, LISTEN_QUEUE_SIZE);
    if (listenResult < 0) {
        error(1, errno, "listen failed");
    }

    return listenFd;
}

void tcp_server_make_socket_reuse(int fd) {
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

void tcp_server_make_socket_nonblock(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

int tcp_server_on_tcp_connect(void *data) {
    TcpServer *tcpServer = (TcpServer *) data;
    ThreadPool *threadPool = tcpServer->threadPool;
    assert( threadPool != NULL);
    int fd = tcpServer->listenFd;
    
    struct sockaddr_in connSocketAddr;
    socklen_t connSocketAddrLen = sizeof(connSocketAddr);
    bzero(&connSocketAddr, sizeof(connSocketAddr));
    int connectFd = accept(fd, (struct sockaddr *) &connSocketAddr, &connSocketAddrLen);
    if (connectFd < 0) {
        error(1, errno, "accept failed");
    }
    tcp_server_make_socket_nonblock(connectFd);

    struct EventLoop *eventLoop = thread_pool_get_loop(threadPool);
    TcpConnection *tcpConnection = malloc(sizeof(TcpConnection));
    tcpConnection->connectFd = connectFd;
    tcpConnection->eventLoop = eventLoop;
    struct Channel *channel = channel_init(connectFd, EVENT_READ, onRead, onWrite, tcpConnection);
    event_loop_add_channel_event(eventLoop, connectFd, channel);
    return 0;
}
