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

/*! \file fate/sys.h
 *  \brief Crash handling, stack traces, and more.
 *  \defgroup sys Sys : Crash handling, stack traces, and more.
 *
 * TODO
 *
 * @{
 */

#ifndef FE_SYS_H
#define FE_SYS_H

#include <stdbool.h>
#include <stdint.h>
#include <fate/log.h>

#define FE_SYS_STACK_LEN 128
#define FE_SYS_MODNAME_LEN 256

/*! \brief TODO */
char *fe_sys_getgamepath(void);
/*! \brief TODO */
void fe_sys_crash_handler_setup(void);
/*! \brief Log a platform-specific stack trace, through the given log function.
 *
 */
void fe_sys_log_stacktrace(fe_logfunc logfunc);

/* @} */
#endif /* FE_SYS_H */
