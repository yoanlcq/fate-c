#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <stdarg.h>

void fate_logf_err(const char *fmt, ...);
void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...));

#endif /* FATE_LOG_H */
