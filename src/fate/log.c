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
#include <fate/dbg.h>
#include <fate/mt.h>
#include <SDL2/SDL.h>
#include <fate/globalstate.h>
#include <fate/i18n.h>
#include <fate/log.h>
#ifdef FE_TARGET_ANDROID
#include <android/log.h>
#elif defined(FE_TARGET_EMSCRIPTEN)
#include <emscripten.h>
#endif

/*
#ifdef FE_ANDROID
#include <android/log.h>
#define  LOG_TAG    "log"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

*/

static unsigned long static_logflags = 0;
void fe_log_setup(void) {}
void fe_log_cleanup(void) {}
void fe_log_flags(unsigned long flags) {
    static_logflags = flags;
    /* TODO: flags should take effect. */
}
unsigned long fe_log_getflags(void) { return static_logflags; }
void fe_log_no_console(const char *tag) {}
void fe_log_on_console(const char *tag) {}
void fe_log_file(const char *tag, const char *filename) {}

#ifdef FE_TARGET_ANDROID
#define log_do_print(constream, sev) \
        __android_log_vprint(ANDROID_LOG_##sev, tag, fmt, vl)
#else
#define log_do_print(constream, sev) \
        fprintf(constream, "%s: ", tag); \
        vfprintf(constream, fmt, vl)
#endif

#define log_helper(constream, sev) \
    va_list vl; \
    va_start(vl, fmt); \
    log_do_print(constream, sev); \
    va_end(vl)

void fe_logi(const char *tag, const char *fmt, ...) {
    log_helper(stdout, INFO);
}
void fe_logw(const char *tag, const char *fmt, ...) {
    log_helper(stderr, WARN);
}
void fe_loge(const char *tag, const char *fmt, ...) {
    log_helper(stderr, ERROR);
}
#ifdef FE_DEBUG_BUILD
void fe_logd(const char *tag, const char *fmt, ...) {
    log_helper(stdout, DEBUG);
}
#endif
void fe_logd_not_macro(const char *tag, const char *fmt, ...) {
#ifdef FE_DEBUG_BUILD
    log_helper(stdout, DEBUG);
#endif
}
#ifdef FE_LOG_USE_VERBOSE
void fe_logv(const char *tag, const char *fmt, ...) {
    log_helper(stdout, VERBOSE);
}
#endif
void fe_logv_not_macro(const char *tag, const char *fmt, ...) {
#ifdef FE_LOG_USE_VERBOSE
    log_helper(stdout, VERBOSE);
#endif
}
void fe_logc(const char *tag, const char *fmt, ...) {
    char message[4096*4];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(message, sizeof(message), fmt, ap);
    va_end(ap);

    log_helper(stderr, ERROR);

#ifdef FE_TARGET_EMSCRIPTEN
    const char *errstr = "F.A.T.E has encountered an error "
                           "from which it cannot recover.";
    fe_dbg_assert((strlen(errstr)+12) < 512);
    char script[512+sizeof(message)];
    snprintf(script, sizeof(script), "alert('%s\n%s');", errstr, message);
    emscripten_run_script(script);
#else
    int button_id = 0xdead;
    const SDL_MessageBoxButtonData button = {
        SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, button_id, "OK"
    };
    const SDL_MessageBoxData mbdat = {
        SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, NULL, 
        "F.A.T.E internal error", message, 1, &button, NULL
    };
#ifndef FE_MT_DISABLE
    /* XXX if fe_logc is called from any thread other than the main thread,
     * then we should enqueue the message box as a request to the main thread. */
    fe_dbg_hope(fe_mt_get_self_id() == fe_mt_get_main_id());
#endif
    SDL_ShowMessageBox(&mbdat, &button_id);
#endif
}
