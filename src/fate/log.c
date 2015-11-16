#include <fate/log.h>

#include <stdio.h>

void fate_logf_err(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
