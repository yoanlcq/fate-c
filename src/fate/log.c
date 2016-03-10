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

/*
#ifdef FATE_ANDROID
#include <android/log.h>
#define  LOG_TAG    "log"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

INFO     green  - Supposed to be compiled in.
WARN     yellow - Supposed to be compiled in.
ERROR    red    - Supposed to be compiled in.
DEBUG    blue   - Supposed to be compiled only for debug builds.
CRITICAL red    - The user must see it by all means, no matter what, but
                  it doesn't seem fatal. Should spawn a message box.

Each module's .c file should declare :
    static const char *TAG = "ThisModule";
Then use :
    fate_logv(TAG, "Blah %d\n", 12, ...);

*/

void fate_log_setup(void) {}
void fate_log_cleanup(void) {}
void fate_log_flags(unsigned long flags) {}
unsigned long fate_log_getflags(void) {}
void fate_log_multiplex(const char *tag, fate_log_severity sev,
                        FILE* streams[], size_t streams_count) {}
void fate_logi(const char *tag, const char *fmt, ...) {}
void fate_logw(const char *tag, const char *fmt, ...) {}
void fate_loge(const char *tag, const char *fmt, ...) {}
void fate_logd(const char *tag, const char *fmt, ...) {}
void fate_logv(const char *tag, const char *fmt, ...) {}
void fate_logc(const char *tag, const char *fmt, ...) {}
void fate_fatal(const char *tag, const char *fmt, ...) {
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


