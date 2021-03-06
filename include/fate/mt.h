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
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr><td>FE_MT_DISABLE</td>
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
 * <tr><td>FE_MT_MUTEX_TRY_GNUTM</td>
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
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FE_MT_MUTEX_TRY_GNUTM
#define FE_MT_MUTEX_TRY_X86HLE
#define FE_MT_MUTEX_TRY_X86RTM

#ifdef FE_TARGET_EMSCRIPTEN
/* It's not like we had the choice anyway. */
#define FE_MT_DISABLE
#endif


typedef int fe_mt_thread;
typedef uint64_t fe_mt_threadid;
typedef SDL_SpinLock fe_mt_spinlock;
typedef SDL_cond* fe_mt_cond;
typedef SDL_mutex* fe_mt_mutex;

typedef enum {
    FE_MT_THREADPRIO_LOW = SDL_THREAD_PRIORITY_LOW,
    FE_MT_THREADPRIO_NORMAL = SDL_THREAD_PRIORITY_NORMAL,
    FE_MT_THREADPRIO_HIGH = SDL_THREAD_PRIORITY_HIGH
} fe_mt_threadpriority;

typedef union {
    fe_mt_mutex as_mutex;
    int32_t     as_hle;
} fe_mt_tsx;

#define FE_MT_MAIN_THREAD_NAME "Main thread"

#ifndef FE_MT_DISABLE

/*! \brief Beware ! Needs fe_hw_setup() first. 
 * It also needs to be called from the main thread, because its ID is used
 * by later calls to #fe_mt_get_main_id().*/
void         fe_mt_setup(void);
void         fe_mt_cleanup(void);

size_t       fe_mt_get_thread_count(void);
void         fe_mt_set_self_priority(fe_mt_threadpriority prio);
fe_mt_threadid fe_mt_get_self_id(void);
fe_mt_threadid fe_mt_get_main_id(void);
fe_mt_thread fe_mt_thread_create(int (*func)(void*), const char *name, void *arg);
void         fe_mt_thread_detach(fe_mt_thread t);
fe_mt_threadid fe_mt_thread_get_id(fe_mt_thread t);
const char * fe_mt_thread_get_name(fe_mt_thread t);
int          fe_mt_thread_wait(fe_mt_thread t);

void fe_mt_mutex_init    (fe_mt_mutex *mutex);
void fe_mt_mutex_deinit  (fe_mt_mutex *mutex);
bool fe_mt_mutex_try_lock(fe_mt_mutex *mutex);
void fe_mt_mutex_lock    (fe_mt_mutex *mutex);
void fe_mt_mutex_unlock  (fe_mt_mutex *mutex);

/* You can't use them recursively. */
bool fe_mt_spinlock_try_lock(fe_mt_spinlock *spinlock);
void fe_mt_spinlock_lock    (fe_mt_spinlock *spinlock);
void fe_mt_spinlock_unlock  (fe_mt_spinlock *spinlock);

/* You can't nest transactions. */
extern void (*fe_mt_tsx_init)  (fe_mt_tsx *t);
extern void (*fe_mt_tsx_deinit)(fe_mt_tsx *t);
extern void (*fe_mt_tsx_lock)  (fe_mt_tsx *t);
extern void (*fe_mt_tsx_unlock)(fe_mt_tsx *t);


void fe_mt_cond_init     (fe_mt_cond *cond);
void fe_mt_cond_deinit   (fe_mt_cond *cond);
void fe_mt_cond_wait     (fe_mt_cond *cond, 
                          fe_mt_mutex *mutex, 
                          uint32_t timeout_milliseconds);
void fe_mt_cond_signal   (fe_mt_cond *cond);
void fe_mt_cond_broadcast(fe_mt_cond *cond);

#ifndef __GNUC__
    #define FE_MT_GNUTM_UNSUPPORTED
#else
    #if FE_VERCMP_LT(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__,4,8,0)
        #define FE_MT_GNUTM_UNSUPPORTED
    #endif
#endif

#ifdef FE_MT_GNUTM_UNSUPPORTED
    #ifdef FE_MT_TSX_TRY_GNUTM
    #undef FE_MT_TSX_TRY_GNUTM
    #endif
#endif

#include <fate/hw.h>

#ifndef FE_HW_TARGET_X86
    #ifdef FE_MT_TSX_TRY_X86RTM
    #undef FE_MT_TSX_TRY_X86RTM
    #endif
    #ifdef FE_MT_TSX_TRY_X86HLE
    #undef FE_MT_TSX_TRY_X86HLE
    #endif
#endif

/* Implementation time ! */

#if defined(FE_MT_TSX_TRY_GNUTM)
    #define fe_mt_tsx_init(tsx)
    #define fe_mt_tsx_deinit(tsx)
    #define fe_mt_tsx_lock(tsx)   __transaction_relaxed {
    #define fe_mt_tsx_unlock(tsx) }
#endif


#else /* FE_MT_DISABLE */

#define fe_mt_setup()
#define fe_mt_cleanup()

#define fe_mt_get_thread_count() (1)
#define fe_mt_set_self_priority()
#define fe_mt_thread_create(f, n, a) (-1)
#define fe_mt_thread_detach(t)
#define fe_mt_thread_get_id(t) 0
#define fe_mt_thread_get_name(t) FE_MT_MAIN_THREAD_NAME
#define fe_mt_thread_wait(t)

#define fe_mt_mutex_init(t)
#define fe_mt_mutex_deinit(t)
#define fe_mt_mutex_try_lock(t)
#define fe_mt_mutex_lock(t)
#define fe_mt_mutex_unlock(t)

#define fe_mt_spinlock_try_lock(t)
#define fe_mt_spinlock_lock(t)
#define fe_mt_spinlock_unlock(t)

#define fe_mt_tsx_init(t)
#define fe_mt_tsx_deinit(t)
#define fe_mt_tsx_lock(t)
#define fe_mt_tsx_unlock(t)

#endif /* FE_MT_DISABLE */

/*! @} */

#endif /* FE_MT_H */
