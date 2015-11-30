#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <stdarg.h>

extern void fate_logf_dummy(const char *fmt, ...);

#define FATE_LOGFUNCDECL(_what_) \
    extern void fate_logf##_what_##_to_console(const char *fmt, ...); \
    extern void fate_logf##_what_##_to_stream(const char *fmt, ...); \
    extern void fate_logf##_what_##_to_console_and_stream(const char *fmt, ...);\
    extern void (*fate_logf##_what_)(const char *fmt, ...)

FATE_LOGFUNCDECL();
FATE_LOGFUNCDECL(_trace);
FATE_LOGFUNCDECL(_err);
FATE_LOGFUNCDECL(_video);
FATE_LOGFUNCDECL(_audio);

#undef FATE_LOGFUNCDECL

#endif /* FATE_LOG_H */
