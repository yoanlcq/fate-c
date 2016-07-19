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
 */


/*! \file fate/linuxperf.h
 *  \brief Wrapper around Linux's Perf Event API.
 *  \defgroup linuxperf Wrapper around Linux's Perf Event API.
 *
 * Wishlist :
 * - Support sampling events;
 * - Support vendor-specific "raw" events (using \c libpfm4 as a reference);
 *
 * This module provides an object which can measure some cool
 * performance-related stats on one or more CPUs. As you may
 * have guessed, it is only available when compiling for Linux.
 *
 * Performance counter APIs vary greatly per-OS, so in the future
 * there might very well be a similar module only for Windows, etc.
 *
 * Linux's Perf Event API allows measuring a huge amount of things -
 * this module here limits itself to the so-called <i>counting events</i>,
 * but not the <i>sampling events</i>. The #fe_linuxperf objects, 
 * discussed more in detail below, make a best effort to use all
 * of the provided counting events, even though it's very likely that most
 * of them won't report anything, either because your hardware doesn't
 * support them, or for a variety of other reasons.
 *
 * Here's a quick start guide to this module :
 *
 * 0. Call #fe_linuxperf_setup() once before using this module;
 * 1. Initialize a monitor with #fe_linuxperf_init();
 * 2. Start profiling with #fe_linuxperf_restart();
 * 3. Do some work;
 * 4. End profiling with #fe_linuxperf_retrieve(). This captures
 *    the counters' values in a #fe_linuxperf_state structure.
 *    If you wish, you can use #fe_linuxperf_state_visit() 
 *    to easily display it.
 * 5. Go back to step 2 as many times as you want;
 * 6. Deinitialize the monitor with #fe_linuxperf_deinit(). 
 *
 * @{
 */

#ifndef FE_LINUXPERF_H
#define FE_LINUXPERF_H

#include <fate/defs.h>
#if defined(FE_TARGET_LINUX) || __DOXYGEN__

#include <stddef.h> /* size_t */
#include <stdbool.h>
#include <stdint.h>

/*! \brief Per-counter data. See #fe_linuxperf_state.  */
typedef struct {
    uint64_t value; /*!< The counter's value. Duh ! */
    uint64_t time_enabled; /*!< 
    * Time, in nanoseconds, since the counter was started (that is, 
    * since the last related call to #fe_linuxperf_init()).
    *
    * This member is NOT reset after a call to #fe_linuxperf_restart() -
    * This is how Linux Perf Events is designed.
    */
    uint64_t time_running; /*!< 
    * For how many nanoseconds the counter was _actually_ on the 
    * CPU, counting stuff. Beware ! Zero is a valid value for this member.
    *
    * https://lwn.net/Articles/324756/ says that a counter's value should
    * be more precisely computed as :\n
    *     <tt>value * time_enabled / time_running</tt>\n
    * Mind the possible division by zero !
    *
    * This member is NOT reset after a call to #fe_linuxperf_restart() -
    * This is how Linux Perf Events is designed.
    */
    int error; /*!<
    * This member is the value of \c errno when one of the following fails :
    * - \c perf_event_open() (in #fe_linuxperf_init());
    * - \c read() (in #fe_linuxperf_retrieve());
    *
    * Otherwise, it is set to zero, and the other members are supposed to
    * have reliable values.
    */
} fe_linuxperf_state_counter;

typedef struct {
    fe_linuxperf_state_counter total_cycles;
    fe_linuxperf_state_counter instructions;
    fe_linuxperf_state_counter cache_accesses;
    fe_linuxperf_state_counter cache_misses;
    fe_linuxperf_state_counter branch_instructions;
    fe_linuxperf_state_counter branch_mispredictions;
    fe_linuxperf_state_counter bus_cycles;
    fe_linuxperf_state_counter stalled_cycles_frontend;
    fe_linuxperf_state_counter stalled_cycles_backend;
    fe_linuxperf_state_counter total_cycles_no_frequency_scaling;
} fe_linuxperf_state_hw; /*!< Hardware state_counters */

typedef struct {
    fe_linuxperf_state_counter page_faults;
    fe_linuxperf_state_counter context_switches;
    fe_linuxperf_state_counter cpu_migrations;
    fe_linuxperf_state_counter minor_page_faults;
    fe_linuxperf_state_counter major_page_faults;
} fe_linuxperf_state_sw; /*!< Software state_counters */

typedef struct {
    fe_linuxperf_state_counter accesses, misses;
} fe_linuxperf_state_cacheop;

typedef struct {
    fe_linuxperf_state_cacheop read, write, prefetch;
} fe_linuxperf_state_cacheio;

typedef struct {
    fe_linuxperf_state_cacheio l1d; /*!< Level 1 Data cache */
    fe_linuxperf_state_cacheio l1i; /*!< Level 1 Instruction cache */
    fe_linuxperf_state_cacheio ll; /*!< Last-Level cache */
    fe_linuxperf_state_cacheio dtlb; /*!< Data TLB */
    fe_linuxperf_state_cacheio itlb; /*!< Instruction TLB */
    fe_linuxperf_state_cacheio bpu; /*!< Branch Prediction Unit */
    fe_linuxperf_state_cacheio local_memory; /*!< Local memory accesses */
} fe_linuxperf_state_cache;

/*! \brief Data collected by a #fe_linuxperf object.
 *
 * See the man page for \c perf_event_open() for more info about each fields.
 */
typedef struct {
    fe_linuxperf_state_hw hw;
    fe_linuxperf_state_sw sw;
    fe_linuxperf_state_cache cache;
} fe_linuxperf_state;

/*! \brief Opaque struct holding the file descriptors for
 *         a set of all existing counting events. */
typedef struct fe_linuxperf fe_linuxperf;

/*! \brief Call once before using the \c fe_linuxperf module. 
 *
 * This simply tests if the user's kernel supports Perf Events.
 * If not, \c false is returned and the functions of this module
 * do nothing, with the following exceptions: 
 * - #fe_linuxperf_retrieve() would zero each counter's content and
 *   set their \c error member to \c ENOSYS;
 * - #fe_linuxperf_state_visit() would still work.
 */
bool fe_linuxperf_setup(void);
/*! \brief Initializes a #fe_linuxperf object.
 *
 * Once you're done using the object, free its associated resources with
 * #fe_linuxperf_deinit().
 *
 * \param cpu_index Pretty much self-explanatory. -1 is a special value
 *        which means "All CPUs at the same time".
 */
void fe_linuxperf_init(fe_linuxperf *pc, int cpu_index);
/*! \brief Deinitializes an initialized #fe_linuxperf object. */
void fe_linuxperf_deinit(fe_linuxperf *pc);
/*! \brief Resets all counters associated with one or 
 *         more initialized #fe_linuxperf objects simultaneously.
 *
 * \param pc Array of one or more initialized #fe_linuxperf objects.
 * \param count Number of elements in \p pc.
 */
void fe_linuxperf_restart(fe_linuxperf *pc, size_t count);
/*! \brief Retrieves the values of all counters associated with
 *         one or more initialized #fe_linuxperf objects simultaneously.
 *
 * \param pc Array of one or more initialized #fe_linuxperf objects.
 * \param res Output parameter : Array of one or more #fe_linuxperf_state
 *            structures, which must contain at least as many 
 *            elements as \p pc.
 * \param count Number of elements in \p pc.
 */
void fe_linuxperf_retrieve(fe_linuxperf *pc, fe_linuxperf_state *res, 
                           size_t count);

/*! \brief Performs a given action for each member 
 *         of a #fe_linuxperf_state structure. 
 */
void fe_linuxperf_state_visit(fe_linuxperf_state *res, void(*visit)(const char *name, fe_linuxperf_state_counter *val));

/*! @} */

/* Private API - Users, do not touch ! */

enum fe_linuxperf_fd {
    FE_LINUXPERF_FD_TOTAL_CYCLES = 0,
    FE_LINUXPERF_FD_INSTRUCTIONS,
    FE_LINUXPERF_FD_CACHE_ACCESSES,
    FE_LINUXPERF_FD_CACHE_MISSES,
    FE_LINUXPERF_FD_BRANCH_INSTRUCTIONS,
    FE_LINUXPERF_FD_BRANCH_MISPREDICTIONS,
    FE_LINUXPERF_FD_BUS_CYCLES,
    FE_LINUXPERF_FD_STALLED_CYCLES_FRONTEND,
    FE_LINUXPERF_FD_STALLED_CYCLES_BACKEND,
    FE_LINUXPERF_FD_TOTAL_CYCLES_NO_FREQUENCY_SCALING,
    FE_LINUXPERF_FD_PAGE_FAULTS,
    FE_LINUXPERF_FD_CONTEXT_SWITCHES,
    FE_LINUXPERF_FD_CPU_MIGRATIONS,
    FE_LINUXPERF_FD_MINOR_PAGE_FAULTS,
    FE_LINUXPERF_FD_MAJOR_PAGE_FAULTS,
    FE_LINUXPERF_FD_L1D_READ_ACCESSES,
    FE_LINUXPERF_FD_L1D_WRITE_ACCESSES,
    FE_LINUXPERF_FD_L1D_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_L1D_READ_MISSES,
    FE_LINUXPERF_FD_L1D_WRITE_MISSES,
    FE_LINUXPERF_FD_L1D_PREFETCH_MISSES,
    FE_LINUXPERF_FD_L1I_READ_ACCESSES,
    FE_LINUXPERF_FD_L1I_WRITE_ACCESSES,
    FE_LINUXPERF_FD_L1I_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_L1I_READ_MISSES,
    FE_LINUXPERF_FD_L1I_WRITE_MISSES,
    FE_LINUXPERF_FD_L1I_PREFETCH_MISSES,
    FE_LINUXPERF_FD_LL_READ_ACCESSES,
    FE_LINUXPERF_FD_LL_WRITE_ACCESSES,
    FE_LINUXPERF_FD_LL_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_LL_READ_MISSES,
    FE_LINUXPERF_FD_LL_WRITE_MISSES,
    FE_LINUXPERF_FD_LL_PREFETCH_MISSES,
    FE_LINUXPERF_FD_DTLB_READ_ACCESSES,
    FE_LINUXPERF_FD_DTLB_WRITE_ACCESSES,
    FE_LINUXPERF_FD_DTLB_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_DTLB_READ_MISSES,
    FE_LINUXPERF_FD_DTLB_WRITE_MISSES,
    FE_LINUXPERF_FD_DTLB_PREFETCH_MISSES,
    FE_LINUXPERF_FD_ITLB_READ_ACCESSES,
    FE_LINUXPERF_FD_ITLB_WRITE_ACCESSES,
    FE_LINUXPERF_FD_ITLB_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_ITLB_READ_MISSES,
    FE_LINUXPERF_FD_ITLB_WRITE_MISSES,
    FE_LINUXPERF_FD_ITLB_PREFETCH_MISSES,
    FE_LINUXPERF_FD_BPU_READ_ACCESSES,
    FE_LINUXPERF_FD_BPU_WRITE_ACCESSES,
    FE_LINUXPERF_FD_BPU_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_BPU_READ_MISSES,
    FE_LINUXPERF_FD_BPU_WRITE_MISSES,
    FE_LINUXPERF_FD_BPU_PREFETCH_MISSES,
    FE_LINUXPERF_FD_NODE_READ_ACCESSES,
    FE_LINUXPERF_FD_NODE_WRITE_ACCESSES,
    FE_LINUXPERF_FD_NODE_PREFETCH_ACCESSES,
    FE_LINUXPERF_FD_NODE_READ_MISSES,
    FE_LINUXPERF_FD_NODE_WRITE_MISSES,
    FE_LINUXPERF_FD_NODE_PREFETCH_MISSES,
    FE_LINUXPERF_FD_COUNT
};

struct fe_linuxperf {
    int fds[FE_LINUXPERF_FD_COUNT];
    int err[FE_LINUXPERF_FD_COUNT];
};


#endif /* FE_TARGET_LINUX */
#endif /* FE_LINUXPERF_H */
