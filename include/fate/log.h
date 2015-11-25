#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <stdarg.h>

extern void (*fate_logf)(const char *fmt, ...);
extern void (*fate_logf_trace)(const char *fmt, ...);
extern void (*fate_logf_err)(const char *fmt, ...);
extern void (*fate_logf_video)(const char *fmt, ...);
extern void (*fate_logf_audio)(const char *fmt, ...);
void fate_log_stacktrace(void (*logfunc)(const char *fmt, ...));


#endif /* FATE_LOG_H */
