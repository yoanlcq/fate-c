#include <stdio.h>
#include <stdarg.h>
#include <SDL2/SDL.h>
#include <fate/globalstate.h>
#include <fate/log.h>

void fate_fatal(const char *fmt, ...) {
    char str[BUFSIZ];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(str, BUFSIZ, fmt, ap);
    va_end(ap);
    if(!fate_logf_err)
        fate_logf_err = fate_logf_err_to_console;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    fate_logf_err(str);
#pragma GCC diagnostic pop
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
        "F.A.T.E internal error", str, NULL);
    fate_globalstate_deinit(fate_gs);
    exit(EXIT_FAILURE);
}

void fate_logf_dummy(const char *fmt, ...) {}

#define FATE_LOGFUNCDECL(_what_,_constream_) \
void fate_log##_what_##_to_console(const char *fmt, ...) { \
    va_list ap; \
    va_start(ap, fmt); \
    vfprintf(_constream_, fmt, ap); \
    va_end(ap); \
} \
void fate_log##_what_##_to_stream(const char *fmt, ...) { \
    va_list ap; \
    va_start(ap, fmt); \
    vfprintf(fate_gs->log##_what_##_stream, fmt, ap); \
    va_end(ap); \
} \
void fate_log##_what_##_to_console_and_stream(const char *fmt, ...) { \
    va_list ap; \
    va_start(ap, fmt); \
    vfprintf(_constream_, fmt, ap); \
    va_end(ap); \
    va_start(ap, fmt); \
    vfprintf(fate_gs->log##_what_##_stream, fmt, ap); \
    va_end(ap); \
} \
void (*fate_log##_what_)(const char *fmt, ...) = fate_log##_what_##_to_console

FATE_LOGFUNCDECL(f,stdout);
FATE_LOGFUNCDECL(f_trace,stderr);
FATE_LOGFUNCDECL(f_err,stderr);
FATE_LOGFUNCDECL(f_video,stdout);
FATE_LOGFUNCDECL(f_audio,stdout);

#undef FATE_LOGFUNCDECL

