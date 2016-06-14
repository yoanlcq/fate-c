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

/*! \file fate/mt.h
 *  \brief MT : Multithreading, Locks/hardware transactional memory, etc.
 *  \defgroup mt MT : Multithreading, Locks/hardware transactional memory, etc.
 *
 * F.A.T.E uses SDL2 as the thread/mutex implementation by default.
 *
 * However, things evolve, and today we have cool things such as
 * Hardware Transactional Memory, which F.A.T.E is able to use instead of
 * regular mutexes, if compiled with the appropriate compile-time switches.
 *
 * This module can also detect deadlocks, if compiled with
 * \c FE_MT_MUTEX_CHECK_DEADLOCKS.<br>
 * In that case, deadlock detection takes place no matter the mutex 
 * implementation (even with Transactional Memory).
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr><td>FE_NO_MT</td>
 *     <td>If defined, guarantees a single threaded program, even preventing
 *         creation of other threads at runtime.<br>
 *         Most functionalities provided by this module then become macros that 
 *         expand to nothing, but behaviour is kept consistent.<br>
 *         For instance, #fe_mt_get_num_threads() will expand to 1, as 
 *         expected.<br>
 *         We may want this in case the overhead of having multiple threads
 *         is comparable to the game's workload itself.<br>
 *         Besides, threads are just not supported on Emscripten as of today
 *         (Feb 2016) and Web Workers is not targeted by F.A.T.E.
 *         Thus, on Emscripten, we don't want mutexes to even <i>exist</i>
 *         within the program. On the other hand, a lot of work is said
 *         to be performed asynchronously, but this is not exactly the same 
 *         thing.
 *         </td></tr>
 * <tr><td>FE_MT_MUTEX_CHECK_DEADLOCKS</td>
 *     <td>Check for deadlocks, regardless of the actual mutex implementation.
 *     </td></tr>
 * <tr><td>FE_MT_MUTEX_USE_GNUTM</td>
 *     <td>Compile on GCC with \c -fgnu-tm. The program then depends on
 *         a GCC runtime library called \c libitm.<br>
 *         GNU Transactional Memory is by far the most portable and safe 
 *         choice, but since it may be implemented as regular library calls, 
 *         it is perhaps not more efficient than direct RTM/HLE instructions 
 *         on x86.</td></tr>
 * <tr><td>FE_MT_MUTEX_TRY_X86HLE</td>
 *     <td>Compile on GCC with \c -mhle. Has no effect if the target is
 *         not an x86.<br>
 *         HLE (Hardware Lock Elision) is a CPU feature, so support for it
 *         is not known until runtime, hence the \c TRY.<br>
 *         If the target is x86, but HLE is not supported, then
 *         the mutex implementation falls back to SDL2's.</td></tr>
 * <tr><td>FE_MT_MUTEX_TRY_X86RTM</td>
 *     <td>Compile on GCC with \c -mrtm. Has no effect if the target is
 *         not an x86.<br>
 *         RTM (Restricted Transactional Memory) is a CPU feature, so 
 *         support for it is not known until runtime, hence the \c TRY.<br>
 *         If the target is x86, but RTM is not supported, then
 *         the mutex implementation falls back to HLE.
 *         </td></tr>
 * </table>
 *
 * <h3>FAQ/discussion :</h3>
 * <b>Is OpenMP support considered ?</b><br>
 * No. Even though OpenMP is a very powerful way
 * of parallelizing various sections within code in a safe manner,
 * the problem is, it does not lend itself easily to a task graph 
 * implementation. <br>
 * Worse : because it works with \c #pragma, if we choose
 * to support OpenMP, then <i>the whole source code</i> has to be adapted, 
 * making it heavily dependent and bloated. 
 * It breaks one the essential "What if"s : "What if we want to change our
 * implementation ?".
 *
 * <b>What about Cilk ?</b><br>
 * No, for the same reasons as above.
 *
 * <b>What about Intel® TBB ?</b><br>
 * I don't know. Sounds like a huge dependency to me, and I really don't
 * feel like writing a C binding for it. Nobody will mind you doing it though!
 *
 * <b>What about [insert thread library here] ?</b><br>
 * Interesting ! But it's pretty much pointless when we have SDL2.
 *
 * <b>How about binding C++'s <tt>std::thread</tt> to C calls ?</b><br>
 * Seems doable, but ugly. Plus it's not as widely supported as you'd expect 
 * from a so-called "standard feature".<br>
 * I'm pretty sure that even SDL2's routines are more portable.<br>
 * The whole thing is broken anyway. There's no such thing as
 * "standard threads" - most of the time they are just implemented with
 * \c pthread library calls, which in turn use OS-specific functions that
 * anybody could just use directly.<br>
 * Yeah, this ain't Java.
 *
 * @{
 */

#ifndef FE_MT_H
#define FE_MT_H

#include <fate/defs.h>

#ifdef FE_EMSCRIPTEN
/* It's not like we had the choice anyway. */
#define FE_NO_MT
#endif

#ifdef FE_NO_MT
#endif

typedef int fe_mt_thread;
typedef int fe_mt_mutex;
FE_NIY void fe_mt_setup(void);
FE_NIY size_t fe_mt_get_num_threads(void);
FE_NIY fe_mt_thread* fe_mt_thread_create(int (*func)(void*), const char *name);
FE_NIY void fe_mt_thread_detach(fe_mt_thread *t);
FE_NIY int fe_mt_thread_getid(fe_mt_thread *t);
FE_NIY const char *fe_mt_thread_getname(fe_mt_thread *t);
FE_NIY void fe_mt_thread_setpriority(float priority);
FE_NIY void fe_mt_thread_wait(fe_mt_thread *t);

FE_NIY void fe_mutex_setup(void);
#define FE_MUTEX_DECL(name) something... 
FE_NIY void fe_mutex_init(fe_mt_mutex *mutex);
FE_NIY void fe_mutex_deinit(fe_mt_mutex *mutex);
FE_NIY void fe_mutex_lock(fe_mt_mutex *mutex);
FE_NIY void fe_mutex_unlock(fe_mt_mutex *mutex);



#ifndef __GNUC__
    #define NOPE
#else
    #if FE_VERCMP_LT(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__,4,8,0)
        #define NOPE
    #endif
#endif

#ifdef NOPE
    #ifdef FE_MT_MUTEX_USE_GNUTM
    #undef FE_MT_MUTEX_USE_GNUTM
    #endif
    #ifdef FE_MT_MUTEX_TRY_X86RTM
    #undef FE_MT_MUTEX_TRY_X86RTM
    #endif
    #ifdef FE_MT_MUTEX_TRY_X86HLE
    #undef FE_MT_MUTEX_TRY_X86HLE
    #endif
    #ifndef FE_MT_MUTEX_USE_SDL2
    #define FE_MT_MUTEX_USE_SDL2
    #endif
#endif

#undef NOPE


/* Can we use x86 builtins at all ? */
#ifndef FE_X86
    #ifdef FE_MT_MUTEX_TRY_X86RTM
    #undef FE_MT_MUTEX_TRY_X86RTM
    #endif
    #ifdef FE_MT_MUTEX_TRY_X86HLE
    #undef FE_MT_MUTEX_TRY_X86HLE
    #endif
    #ifndef FE_MT_MUTEX_USE_SDL2
    #define FE_MT_MUTEX_USE_SDL2
    #endif
#endif


/* Implementation time ! */

#ifdef FE_NO_MT

    #define FE_MT_MUTEX_DECL(name) 
    #define fe_mt_mutex_init(mutex)
    #define fe_mt_mutex_deinit(mutex)
    #define fe_mt_mutex_lock(mutex)
    #define fe_mt_mutex_unlock(mutex)

#elif defined(FE_MT_MUTEX_USE_GNUTM)
#include "mt/mutex_gnutm.h"
#elif defined(FE_MT_MUTEX_USE_SDL2)
#include "mt/mutex_sdl2.h"
#endif

/* Nice documentation for x86 intrinsics : 
 * https://software.intel.com/sites/landingpage/IntrinsicsGuide */

#if defined(FE_MT_MUTEX_TRY_X86HLE) || defined(FE_MT_MUTEX_TRY_X86RTM)
    #ifdef _MSVC_VER
        #include <intrin.h>
    #else
        #include <immintrin.h>
    #endif
#endif

#ifdef FE_MT_MUTEX_TRY_X86HLE
#include "mt/mutex_x86hle.h"
#endif

#ifdef FE_MT_MUTEX_TRY_X86RTM
#include "mt/mutex_x86rtm.h"
#endif

/*! @} */

#endif /* FE_MT_H */
