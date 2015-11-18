#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <stdarg.h>

void fate_logf_err(const char *fmt, ...);
extern void (*fate_logf_trace)(const char *fmt, ...);
void fate_logf_trace_real(const char *fmt, ...);
void fate_logf_dummy(const char *fmt, ...);
void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...));


#endif /* FATE_LOG_H */
