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

/*! \file fate/log.h
 *  \brief The logging module, multiplexing output.
 *  \defgroup log Logging module
 *
 *  TODO
 */

#ifndef FATE_LOG_H
#define FATE_LOG_H

#include <fate/defs_compiler.h>
#include <stdarg.h>

void fate_fatal(const char *fmt, ...);
void fate_logf_dummy(const char *fmt, ...);

#define FATE_LOGFUNCDECL(_what_) \
    void fate_log##_what_##_to_console(const char *fmt, ...); \
    void fate_log##_what_##_to_stream(const char *fmt, ...); \
    void fate_log##_what_##_to_console_and_stream(const char *fmt, ...); \
    extern void (*fate_log##_what_)(const char *fmt, ...) \
        FATE_PRINTF_DECL

FATE_LOGFUNCDECL(f);
FATE_LOGFUNCDECL(f_trace);
FATE_LOGFUNCDECL(f_err);
FATE_LOGFUNCDECL(f_video);
FATE_LOGFUNCDECL(f_audio);

#undef FATE_LOGFUNCDECL

#endif /* FATE_LOG_H */
