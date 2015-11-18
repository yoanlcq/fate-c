#include <stdio.h>
#include <fate/log.h>

void fate_logf_err(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

void (*fate_logf_trace)(const char *fmt, ...) = fate_logf_trace_real;

void fate_logf_trace_real(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "\033[0;36m");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\033[m");
    va_end(ap);
}

void fate_logf_dummy(const char *fmt, ...) {}
