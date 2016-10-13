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

#ifndef FE_DBG_H
#define FE_DBG_H

/*! \file fate/dbg.h
 *  \brief Debug header.
 *  \defgroup dbg Debug header.
 *
 * @{
 */

#include <fate/defs.h>
#include <fate/log.h>
#include <SDL2/SDL.h>

#if __DOXYGEN__ || defined(FE_DEBUG_BUILD)
/*! \brief Triggers a platform-specific debugger breakpoint. 
 * 
 * Only does something when FE_DEBUG_BUILD is defined. */
    #define fe_dbg_break() SDL_TriggerBreakpoint()
#else
    #define fe_dbg_break()
#endif

#if __DOXYGEN__ || defined(FE_DEBUG_BUILD)
/*! \brief Guess what this does ?
 * 
 * This is a macro with the same meaning as #assert(), but with a better behaviour.
 *
 * WARNING. The expression to evaluate must not have any side-effects.
 * Otherwise, the app's behaviour may depend on the kind of build (debug or release).
 *
 * That is, it prints :
 * - The source file's name;
 * - The current function's name;
 * - The current line number in the source file;
 * - A stack trace;
 * The behaviour relies on #fe_fatal(). At the time of this writing, the program is
 * terminated with exit(), because abort() drives the crash handler nuts.
 *
 * You should already know when to use it, but here is a reminder :
 * assert() is to be used when :
 * - You need a condition to be true;
 * - You know the condition could only be false 
 *   because of a careless mistake on your (or a teammate's) part, 
 *   and not because of the app's user.
 *
 * For this reason, this macro is only enabled on debug builds, that is when you're still
 * in the development phase.
 * It is often used to validate function parameters.
 *
 * Good example usage of assert():
 * \code{.c}
 * uint32_t mygame_search_object_by_name(const char *name) {
 *     fe_dbg_assert(name && "We said that 'name' must not be NULL!\n");
 *     // The actual function code goes here.
 * }
 * // However, in this specific case, we could protect ourselves better by
 * // adding FE_DECL_NONNULL_PARAMS(1) to the function's declaration.
 * \endcode
 */
#define fe_dbg_assert(cond) \
    do { \
        if(!(cond)) { \
            fe_fatal("fe_dbg", "%s:%s:%u: Assertion `%s' failed.\n", \
                __FILE__, __func__, __LINE__, #cond); \
        } \
    } while(0)
#else
#define fe_dbg_assert(cond) ((void)(cond))
#endif

#if __DOXYGEN__ || defined(FE_DEBUG_BUILD)
#define fe_dbg_assert_warn(cond) \
    do { \
        if(!(cond)) { \
            fe_logw("fe_dbg", "%s:%s:%u: Assertion `%s' failed.\n", \
                __FILE__, __func__, __LINE__, #cond); \
        } \
    } while(0)
#else
#define fe_dbg_assert_warn(cond) ((void)(cond))
#endif

/*! \brief Shortcut for exiting miserably when a condition is not met.
 * 
 * The difference with #fe_dbg_assert() is that this macro is defined for all builds.
 * Use this macro when you require a condition to be met, but you plan to write
 * the error handling code later because it would distract you from your
 * original intent.
 * Then, whenever the condition is not met, this macro displays everything it can
 * about the app's state.
 *
 * Before shipping your app, you'd search for any remaining instances of this macro in 
 * the codebase, and replace each of them by proper error handling.
 * That's why it's called "hope" instead of "require" and such. It means
 * "I'm too lazy to handle errors for now, so I'll just keep going and hope that everything
 *  goes according to plan."
 * 
 * While #fe_dbg_assert() is meant to quickly catch trivial mistakes made by developers, 
 * this macro is meant to catch unexpected input from both developers and testers.
 *
 * Good example usage of #fe_dbg_hope() :
 * \code{.c}
 * // Note : actual code should use the fe_iov APIs instead.
 * FILE *spr = fopen("data/hero/hero.png", "r");
 * fe_dbg_hope(spr); // "I'll deal with errors later. Shout loud if they ever happen though".
 * // Do stuff with spr...
 * \endcode
 */
#define fe_dbg_hope(cond) \
    do { \
        if(!(cond)) { \
            fe_fatal("fe_dbg", "%s:%s:%u: Hoped that `%s' would be true.\n", \
                     __FILE__, __func__, __LINE__, #cond); \
        }  \
    } while(0)

/*! @} */

#endif /* FE_DBG_H */
