#include <stdio.h>
#include <stdarg.h>
#include <fate/globalstate.h>
#include <fate/log.h>

void fate_logf_dummy(const char *fmt, ...) {}

#define FATE_LOGFUNCDECL(_what_,_constream_) \
void fate_logf##_what_##_to_console(const char *fmt, ...) { \
    va_list ap; \
    va_start(ap, fmt); \
    vfprintf(_constream_, fmt, ap); \
    va_end(ap); \
} \
void fate_logf##_what_##_to_stream(const char *fmt, ...) { \
    va_list ap; \
    va_start(ap, fmt); \
    vfprintf(fate_gs->logf##_what_##_stream, fmt, ap); \
    va_end(ap); \
} \
void fate_logf##_what_##_to_console_and_stream(const char *fmt, ...) { \
    va_list ap; \
    va_start(ap, fmt); \
    vfprintf(_constream_, fmt, ap); \
    vfprintf(fate_gs->logf##_what_##_stream, fmt, ap); \
    va_end(ap); \
} \
void (*fate_logf##_what_)(const char *fmt, ...);

FATE_LOGFUNCDECL(,stdout);
FATE_LOGFUNCDECL(_trace,stderr);
FATE_LOGFUNCDECL(_err,stderr);
FATE_LOGFUNCDECL(_video,stdout);
FATE_LOGFUNCDECL(_audio,stdout);

#undef FATE_LOGFUNCDECL

