/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/*! \file fate/log.c
 *  \brief The logging module's implementation.
 *
 * TODO
 */

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

#ifdef __EMSCRIPTEN__
    char script[BUFSIZ];
    sprintf(script, "alert('F.A.T.E has encountered an internal error from which it cannot recover.\n%s');", str);
    emscripten_run_script(script);
#endif

    if(!fate_logf_err)
        fate_logf_err = fate_logf_err_to_console;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    fate_logf_err(str);
#pragma GCC diagnostic pop
#ifdef FATE_DEBUG
    fate_sys_log_stacktrace(fate_logf_err);
#endif
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

