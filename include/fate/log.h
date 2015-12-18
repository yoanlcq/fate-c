#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <stdarg.h>

void fate_fatal(const char *fmt, ...);
extern void fate_logf_dummy(const char *fmt, ...);

#define FATE_LOGFUNCDECL(_what_) \
    extern void fate_log##_what_##_to_console(const char *fmt, ...); \
    extern void fate_log##_what_##_to_stream(const char *fmt, ...); \
    extern void fate_log##_what_##_to_console_and_stream(const char *fmt, ...);\
    extern void (*fate_log##_what_)(const char *fmt, ...)

FATE_LOGFUNCDECL(f);
FATE_LOGFUNCDECL(f_trace);
FATE_LOGFUNCDECL(f_err);
FATE_LOGFUNCDECL(f_video);
FATE_LOGFUNCDECL(f_audio);

#undef FATE_LOGFUNCDECL

#endif /* FATE_LOG_H */
