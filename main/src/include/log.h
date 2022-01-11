//
// Created by charles.zuo on 2021/12/15.
//

#ifndef ZXCHTTPSERVER_LOG_H
#define ZXCHTTPSERVER_LOG_H

#include <stdio.h>
#define LOG_ENABLE 1
void log_msg(const char *severity, const char *errStr, const char *fmt, va_list ap);

void logd(const char *msg, ...);

void loge(const char *errMsg,const char *msg, ...);
#endif //ZXCHTTPSERVER_LOG_H
