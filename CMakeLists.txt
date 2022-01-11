cmake_minimum_required(VERSION 3.10)
project(ZxcHttpServer C)

set(CMAKE_C_STANDARD 99)

add_executable(ZxcHttpServer main/src/main.c
        main/src/event_loop.c
        main/src/event_loop_thread.c
        main/src/event_dispatcher.c
        main/src/channel.c
        main/src/thread_pool.c
        main/src/log.c
        main/src/channel_map.c
        main/src/tcp_server.c
        main/src/buffer.c main/src/include/buffer.h)