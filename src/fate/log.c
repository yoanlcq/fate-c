#include <stdio.h>
#include <fate/log.h>

void fate_logf_dummy(const char *fmt, ...) {}

#define FATE_MULTIPLEX(_what_,_constream_) \
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
void (*fate_logf##_what_)(const char *fmt, ...) \
    = &fate_logf##_what_##_to_console

FATE_MULTIPLEX(,stdout);
FATE_MULTIPLEX(_trace,stderr);
FATE_MULTIPLEX(_err,stderr);
FATE_MULTIPLEX(_video,stdout);
FATE_MULTIPLEX(_audio,stdout);

#undef FATE_MULTIPLEX

