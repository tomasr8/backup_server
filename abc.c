#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>

void vsyslog(int priority, const char *format, va_list ap);

void log_msg(int prio, char* msg, va_list argptr) {
    va_list argptr_copy;
    va_copy(argptr_copy, argptr);

    vfprintf(stderr, msg, argptr);

    //va_end(argptr);
    vsyslog(prio, msg, argptr_copy);
    va_end(argptr_copy);
}

void log_warn(char *msg, ...) {
    va_list argptr;
    va_start(argptr, msg);
    log_msg(LOG_WARNING, msg, argptr);
    va_end(argptr);
}

int main(void) {
    log_warn("abc %d", 123);
    return 0;
}
