//
// Created by charles.zuo on 2021/12/15.
//

#include <string.h>
#include <stdarg.h>
#include "include/log.h"

void log_msg(const char *severity, const char *errStr, const char *fmt, va_list ap) {
    if (!LOG_ENABLE) {
        return;
    }
    char buf[1024];
    size_t len;
    if (fmt != NULL) {
        vsnprintf(buf, sizeof(buf), fmt, ap);
    } else {
        buf[0] = '\0';
    }

    if (severity == NULL)
        severity = "DEBUG";

    if (errStr) {
        len = strlen(buf);
        if (len < sizeof(buf) - 3) {
            snprintf(buf + len, sizeof(buf) - len, ": %s", errStr);
        }
    }
    fprintf(stdout, "[%s] %s", severity, buf);
}

void logd(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    log_msg("DEBUG", NULL, msg, ap);
    va_end(ap);
}

void loge(const char *errMsg, const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    log_msg("ERROR", errMsg, msg, ap);
    va_end(ap);
}

