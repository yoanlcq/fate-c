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

/*
 * Some notes about the implementation :
 *
 * - It's possible that the Perf Events is not supported at
 *   build time (I'm looking at you, Travis CI). In this case,
 *   FE_LINUXPERF_UNSUPPORTED is defined.
 *
 * - Yeah, I know about the group leader stuff, but I'm avoiding
 *   it on purpose. The thing is, for some events :
 *   - If those are supported at all :
 *     - perf_event_open() would fail when trying to put said 
 *       event in a group;
 *     - But it would succeed when opening said event as its own
 *       group leader.
 *   Since there's no way to know which events can be successfully
 *   put into a group alongside with some others, I've decided
 *   that each counter should be its own group leader, period.
 *   To enable/disable them all at the same time, I use prctl().
 *
 * - Yes, I acknowledge that prctl() enables/disables counters
 *   for the whole process, and I'm fine with it. At least I have
 *   some sort of guarantee that all counters are started at the 
 *   same time, or put another way, it's not my fault if they aren't.
 *   Doesn't seem like a problem even in multi-threaded programs.
 *
 * - perf_event_open() doesn't fail because of some misconfiguration
 *   on my part - I've triple-checked. When it fails, errno is either
 *   set to EINVAL or ENOENT which both can indicate that the event
 *   is not supported.
 *
 * - If it makes a difference, I've tested on my Ubuntu Xenial laptop,
 *   (kernel version : 4.4.0-31-lowlatency) which runs on a Core i5.
 */


#include <fate/defs.h>
#ifdef FE_TARGET_LINUX

#include <fate/linuxperf.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#ifndef FE_LINUXPERF_UNSUPPORTED
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>

#ifdef _GNU_SOURCE
static int perf_event_open(struct perf_event_attr *hw_event,
                    pid_t pid, int cpu, int group_fd, 
                    unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}
#else
#error "Please define _GNU_SOURCE so I can use syscall(), for perf_event_open()."
#endif

#endif


static void dummy_linuxperf_init(fe_linuxperf *pc, int cpu_index) {}
static void (*ptr_linuxperf_init)(fe_linuxperf *pc, int cpu_index) = dummy_linuxperf_init;
void fe_linuxperf_init(fe_linuxperf *pc, int cpu_index) {
    ptr_linuxperf_init(pc, cpu_index);
}
#ifndef FE_LINUXPERF_UNSUPPORTED
static void real_linuxperf_init(fe_linuxperf *pc, int cpu_index) {
    struct perf_event_attr pa[FE_LINUXPERF_FD_COUNT] = {{0}};

    pa[FE_LINUXPERF_FD_TOTAL_CYCLES                     ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_INSTRUCTIONS                     ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_CACHE_ACCESSES                   ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_CACHE_MISSES                     ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_BRANCH_INSTRUCTIONS              ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_BRANCH_MISPREDICTIONS            ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_BUS_CYCLES                       ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_STALLED_CYCLES_FRONTEND          ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_STALLED_CYCLES_BACKEND           ].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_TOTAL_CYCLES_NO_FREQUENCY_SCALING].type = PERF_TYPE_HARDWARE;
    pa[FE_LINUXPERF_FD_PAGE_FAULTS                      ].type = PERF_TYPE_SOFTWARE;
    pa[FE_LINUXPERF_FD_CONTEXT_SWITCHES                 ].type = PERF_TYPE_SOFTWARE;
    pa[FE_LINUXPERF_FD_CPU_MIGRATIONS                   ].type = PERF_TYPE_SOFTWARE;
    pa[FE_LINUXPERF_FD_MINOR_PAGE_FAULTS                ].type = PERF_TYPE_SOFTWARE;
    pa[FE_LINUXPERF_FD_MAJOR_PAGE_FAULTS                ].type = PERF_TYPE_SOFTWARE;
    pa[FE_LINUXPERF_FD_L1D_READ_ACCESSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1D_WRITE_ACCESSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1D_PREFETCH_ACCESSES            ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1D_READ_MISSES                  ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1D_WRITE_MISSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1D_PREFETCH_MISSES              ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1I_READ_ACCESSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1I_WRITE_ACCESSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1I_PREFETCH_ACCESSES            ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1I_READ_MISSES                  ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1I_WRITE_MISSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_L1I_PREFETCH_MISSES              ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_LL_READ_ACCESSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_LL_WRITE_ACCESSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_LL_PREFETCH_ACCESSES             ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_LL_READ_MISSES                   ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_LL_WRITE_MISSES                  ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_LL_PREFETCH_MISSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_DTLB_READ_ACCESSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_DTLB_WRITE_ACCESSES              ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_DTLB_PREFETCH_ACCESSES           ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_DTLB_READ_MISSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_DTLB_WRITE_MISSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_DTLB_PREFETCH_MISSES             ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_ITLB_READ_ACCESSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_ITLB_WRITE_ACCESSES              ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_ITLB_PREFETCH_ACCESSES           ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_ITLB_READ_MISSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_ITLB_WRITE_MISSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_ITLB_PREFETCH_MISSES             ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_BPU_READ_ACCESSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_BPU_WRITE_ACCESSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_BPU_PREFETCH_ACCESSES            ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_BPU_READ_MISSES                  ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_BPU_WRITE_MISSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_BPU_PREFETCH_MISSES              ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_NODE_READ_ACCESSES               ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_NODE_WRITE_ACCESSES              ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_NODE_PREFETCH_ACCESSES           ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_NODE_READ_MISSES                 ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_NODE_WRITE_MISSES                ].type = PERF_TYPE_HW_CACHE;
    pa[FE_LINUXPERF_FD_NODE_PREFETCH_MISSES             ].type = PERF_TYPE_HW_CACHE;

#define CACHECFG(id,op,res) \
    ( (PERF_COUNT_HW_CACHE_##id) \
    | (PERF_COUNT_HW_CACHE_OP_##op << 8) \
    | (PERF_COUNT_HW_CACHE_RESULT_##res << 16))
    pa[FE_LINUXPERF_FD_TOTAL_CYCLES                     ].config = PERF_COUNT_HW_CPU_CYCLES;
    pa[FE_LINUXPERF_FD_INSTRUCTIONS                     ].config = PERF_COUNT_HW_INSTRUCTIONS;
    pa[FE_LINUXPERF_FD_CACHE_ACCESSES                   ].config = PERF_COUNT_HW_CACHE_REFERENCES;
    pa[FE_LINUXPERF_FD_CACHE_MISSES                     ].config = PERF_COUNT_HW_CACHE_MISSES;
    pa[FE_LINUXPERF_FD_BRANCH_INSTRUCTIONS              ].config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
    pa[FE_LINUXPERF_FD_BRANCH_MISPREDICTIONS            ].config = PERF_COUNT_HW_BRANCH_MISSES;
    pa[FE_LINUXPERF_FD_BUS_CYCLES                       ].config = PERF_COUNT_HW_BUS_CYCLES;
    pa[FE_LINUXPERF_FD_STALLED_CYCLES_FRONTEND          ].config = PERF_COUNT_HW_STALLED_CYCLES_FRONTEND;
    pa[FE_LINUXPERF_FD_STALLED_CYCLES_BACKEND           ].config = PERF_COUNT_HW_STALLED_CYCLES_BACKEND;
    pa[FE_LINUXPERF_FD_TOTAL_CYCLES_NO_FREQUENCY_SCALING].config = PERF_COUNT_HW_REF_CPU_CYCLES;
    pa[FE_LINUXPERF_FD_PAGE_FAULTS                      ].config = PERF_COUNT_SW_PAGE_FAULTS;
    pa[FE_LINUXPERF_FD_CONTEXT_SWITCHES                 ].config = PERF_COUNT_SW_CONTEXT_SWITCHES;
    pa[FE_LINUXPERF_FD_CPU_MIGRATIONS                   ].config = PERF_COUNT_SW_CPU_MIGRATIONS;
    pa[FE_LINUXPERF_FD_MINOR_PAGE_FAULTS                ].config = PERF_COUNT_SW_PAGE_FAULTS_MIN;
    pa[FE_LINUXPERF_FD_MAJOR_PAGE_FAULTS                ].config = PERF_COUNT_SW_PAGE_FAULTS_MAJ;
    pa[FE_LINUXPERF_FD_L1D_READ_ACCESSES                ].config = CACHECFG(L1D, READ, ACCESS);
    pa[FE_LINUXPERF_FD_L1D_WRITE_ACCESSES               ].config = CACHECFG(L1D, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_L1D_PREFETCH_ACCESSES            ].config = CACHECFG(L1D, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_L1D_READ_MISSES                  ].config = CACHECFG(L1D, READ, MISS);
    pa[FE_LINUXPERF_FD_L1D_WRITE_MISSES                 ].config = CACHECFG(L1D, WRITE, MISS);
    pa[FE_LINUXPERF_FD_L1D_PREFETCH_MISSES              ].config = CACHECFG(L1D, PREFETCH, MISS);
    pa[FE_LINUXPERF_FD_L1I_READ_ACCESSES                ].config = CACHECFG(L1I, READ, ACCESS);
    pa[FE_LINUXPERF_FD_L1I_WRITE_ACCESSES               ].config = CACHECFG(L1I, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_L1I_PREFETCH_ACCESSES            ].config = CACHECFG(L1I, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_L1I_READ_MISSES                  ].config = CACHECFG(L1I, READ, MISS);
    pa[FE_LINUXPERF_FD_L1I_WRITE_MISSES                 ].config = CACHECFG(L1I, WRITE, MISS);
    pa[FE_LINUXPERF_FD_L1I_PREFETCH_MISSES              ].config = CACHECFG(L1I, PREFETCH, MISS);
    pa[FE_LINUXPERF_FD_LL_READ_ACCESSES                 ].config = CACHECFG(LL, READ, ACCESS);
    pa[FE_LINUXPERF_FD_LL_WRITE_ACCESSES                ].config = CACHECFG(LL, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_LL_PREFETCH_ACCESSES             ].config = CACHECFG(LL, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_LL_READ_MISSES                   ].config = CACHECFG(LL, READ, MISS);
    pa[FE_LINUXPERF_FD_LL_WRITE_MISSES                  ].config = CACHECFG(LL, WRITE, MISS);
    pa[FE_LINUXPERF_FD_LL_PREFETCH_MISSES               ].config = CACHECFG(LL, PREFETCH, MISS);
    pa[FE_LINUXPERF_FD_DTLB_READ_ACCESSES               ].config = CACHECFG(DTLB, READ, ACCESS);
    pa[FE_LINUXPERF_FD_DTLB_WRITE_ACCESSES              ].config = CACHECFG(DTLB, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_DTLB_PREFETCH_ACCESSES           ].config = CACHECFG(DTLB, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_DTLB_READ_MISSES                 ].config = CACHECFG(DTLB, READ, MISS);
    pa[FE_LINUXPERF_FD_DTLB_WRITE_MISSES                ].config = CACHECFG(DTLB, WRITE, MISS);
    pa[FE_LINUXPERF_FD_DTLB_PREFETCH_MISSES             ].config = CACHECFG(DTLB, PREFETCH, MISS);
    pa[FE_LINUXPERF_FD_ITLB_READ_ACCESSES               ].config = CACHECFG(ITLB, READ, ACCESS);
    pa[FE_LINUXPERF_FD_ITLB_WRITE_ACCESSES              ].config = CACHECFG(ITLB, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_ITLB_PREFETCH_ACCESSES           ].config = CACHECFG(ITLB, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_ITLB_READ_MISSES                 ].config = CACHECFG(ITLB, READ, MISS);
    pa[FE_LINUXPERF_FD_ITLB_WRITE_MISSES                ].config = CACHECFG(ITLB, WRITE, MISS);
    pa[FE_LINUXPERF_FD_ITLB_PREFETCH_MISSES             ].config = CACHECFG(ITLB, PREFETCH, MISS);
    pa[FE_LINUXPERF_FD_BPU_READ_ACCESSES                ].config = CACHECFG(BPU, READ, ACCESS);
    pa[FE_LINUXPERF_FD_BPU_WRITE_ACCESSES               ].config = CACHECFG(BPU, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_BPU_PREFETCH_ACCESSES            ].config = CACHECFG(BPU, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_BPU_READ_MISSES                  ].config = CACHECFG(BPU, READ, MISS);
    pa[FE_LINUXPERF_FD_BPU_WRITE_MISSES                 ].config = CACHECFG(BPU, WRITE, MISS);
    pa[FE_LINUXPERF_FD_BPU_PREFETCH_MISSES              ].config = CACHECFG(BPU, PREFETCH, MISS);
    pa[FE_LINUXPERF_FD_NODE_READ_ACCESSES               ].config = CACHECFG(NODE, READ, ACCESS);
    pa[FE_LINUXPERF_FD_NODE_WRITE_ACCESSES              ].config = CACHECFG(NODE, WRITE, ACCESS);
    pa[FE_LINUXPERF_FD_NODE_PREFETCH_ACCESSES           ].config = CACHECFG(NODE, PREFETCH, ACCESS);
    pa[FE_LINUXPERF_FD_NODE_READ_MISSES                 ].config = CACHECFG(NODE, READ, MISS);
    pa[FE_LINUXPERF_FD_NODE_WRITE_MISSES                ].config = CACHECFG(NODE, WRITE, MISS);
    pa[FE_LINUXPERF_FD_NODE_PREFETCH_MISSES             ].config = CACHECFG(NODE, PREFETCH, MISS);
#undef CACHECFG

    size_t i;
    for(i=0 ; i<FE_LINUXPERF_FD_COUNT ; ++i) {
        pa[i].size = sizeof(struct perf_event_attr);
        /* I suppose we want to monitor what we have control over. */
        pa[i].exclude_user = false;
        pa[i].exclude_kernel = true;
        pa[i].exclude_hv = true;
        pa[i].exclude_idle = true;
        pa[i].read_format = PERF_FORMAT_TOTAL_TIME_ENABLED
                          | PERF_FORMAT_TOTAL_TIME_RUNNING;
    }
    /* Special case : Context switches ! */
    pa[FE_LINUXPERF_FD_CONTEXT_SWITCHES].exclude_kernel = false;

    for(i=0 ; i<FE_LINUXPERF_FD_COUNT ; ++i) {
        pc->fds[i] = perf_event_open(pa+i, 0, cpu_index, -1, 0);
        if(pc->fds[i] == -1) {
            pc->err[i] = errno;
            continue;
        }
        /* See BUGS in perf_event_open(2). 
         * Basically, if the user runs on a quite old kernel, OR if
         * the NMI watchdog is enabled, then we should perform a dummy 
         * read() after perf_event_open().
         */
        fe_linuxperf_state_counter fmt;
        ssize_t bytes_read = read(pc->fds[i], &fmt, 3*sizeof(uint64_t));
        if(bytes_read == -1) {
            pc->err[i] = errno;
            continue;
        }
        pc->err[i] = 0;
    }
}
#endif /* FE_LINUXPERF_UNSUPPORTED */


static void dummy_linuxperf_deinit(fe_linuxperf *pc) {}

static void (*ptr_linuxperf_deinit)(fe_linuxperf *pc) = dummy_linuxperf_deinit;

void fe_linuxperf_deinit(fe_linuxperf *pc) {
    ptr_linuxperf_deinit(pc);
}


#ifndef FE_LINUXPERF_UNSUPPORTED
static void real_linuxperf_deinit(fe_linuxperf *pc) {
    size_t i;
    for(i=0 ; i<FE_LINUXPERF_FD_COUNT ; ++i)
        if(!pc->err[i])
            close(pc->fds[i]);
}
#endif /* FE_LINUXPERF_UNSUPPORTED */


static void dummy_linuxperf_restart(fe_linuxperf *pc, size_t count) {}

static void (*ptr_linuxperf_restart)(fe_linuxperf *pc, size_t count) = dummy_linuxperf_restart;

void fe_linuxperf_restart(fe_linuxperf *pc, size_t count) {
    ptr_linuxperf_restart(pc, count);
}


#ifndef FE_LINUXPERF_UNSUPPORTED
static void real_linuxperf_restart(fe_linuxperf *pc, size_t count) {
    size_t i, j;

    prctl(PR_TASK_PERF_EVENTS_DISABLE, 0, 0, 0, 0);
    for(i=0 ; i<count ; ++i)
        for(j=0 ; j<FE_LINUXPERF_FD_COUNT ; ++j)
            if(!pc[i].err[j])
                ioctl(pc[i].fds[j], PERF_EVENT_IOC_RESET, 0);
    prctl(PR_TASK_PERF_EVENTS_ENABLE, 0, 0, 0, 0);
}

static void static_readmember(fe_linuxperf *pc, size_t i, fe_linuxperf_state_counter *member) {
    if(pc->err[i]) {
        memset(member, 0, 3*sizeof(uint64_t)); 
        member->error = pc->err[i];
        return;
    }
    ssize_t nread = read(pc->fds[i], member, 3*sizeof(uint64_t));
    if(nread < 3*sizeof(uint64_t)) {
        member->error = errno*(nread==-1);
        memset(member, 0, 3*sizeof(uint64_t));
        return;
    }
    member->error = 0;
}
#endif /* FE_LINUXPERF_UNSUPPORTED */

static void linuxperf_state_invalidator(const char *name, fe_linuxperf_state_counter *val) {
    val->value = val->time_enabled = val->time_running = 0;
    val->error = ENOSYS;
}
static void dummy_linuxperf_retrieve(fe_linuxperf *pc, fe_linuxperf_state *res, size_t count) {
    size_t i;
    for(i=0 ; i<count ; ++i)
        fe_linuxperf_state_visit(res+i, linuxperf_state_invalidator);
}

static void (*ptr_linuxperf_retrieve)(fe_linuxperf *pc, fe_linuxperf_state *res, size_t count) = dummy_linuxperf_retrieve;

void fe_linuxperf_retrieve(fe_linuxperf *pc, fe_linuxperf_state *res, size_t count) {
    ptr_linuxperf_retrieve(pc, res, count);
}

#ifndef FE_LINUXPERF_UNSUPPORTED
static void real_linuxperf_retrieve(fe_linuxperf *pc, fe_linuxperf_state *res, size_t count) {

    prctl(PR_TASK_PERF_EVENTS_DISABLE, 0, 0, 0, 0);

    size_t i;
    for(i=0 ; i<count ; ++i) {
        static_readmember(pc+i, FE_LINUXPERF_FD_TOTAL_CYCLES                     , &res[i].hw.total_cycles                     );
        static_readmember(pc+i, FE_LINUXPERF_FD_INSTRUCTIONS                     , &res[i].hw.instructions                     );
        static_readmember(pc+i, FE_LINUXPERF_FD_CACHE_ACCESSES                   , &res[i].hw.cache_accesses                   );
        static_readmember(pc+i, FE_LINUXPERF_FD_CACHE_MISSES                     , &res[i].hw.cache_misses                     );
        static_readmember(pc+i, FE_LINUXPERF_FD_BRANCH_INSTRUCTIONS              , &res[i].hw.branch_instructions              );
        static_readmember(pc+i, FE_LINUXPERF_FD_BRANCH_MISPREDICTIONS            , &res[i].hw.branch_mispredictions            );
        static_readmember(pc+i, FE_LINUXPERF_FD_BUS_CYCLES                       , &res[i].hw.bus_cycles                       );
        static_readmember(pc+i, FE_LINUXPERF_FD_STALLED_CYCLES_FRONTEND          , &res[i].hw.stalled_cycles_frontend          );
        static_readmember(pc+i, FE_LINUXPERF_FD_STALLED_CYCLES_BACKEND           , &res[i].hw.stalled_cycles_backend           );
        static_readmember(pc+i, FE_LINUXPERF_FD_TOTAL_CYCLES_NO_FREQUENCY_SCALING, &res[i].hw.total_cycles_no_frequency_scaling);
        static_readmember(pc+i, FE_LINUXPERF_FD_PAGE_FAULTS                      , &res[i].sw.page_faults                      );
        static_readmember(pc+i, FE_LINUXPERF_FD_CONTEXT_SWITCHES                 , &res[i].sw.context_switches                 );
        static_readmember(pc+i, FE_LINUXPERF_FD_CPU_MIGRATIONS                   , &res[i].sw.cpu_migrations                   );
        static_readmember(pc+i, FE_LINUXPERF_FD_MINOR_PAGE_FAULTS                , &res[i].sw.minor_page_faults                );
        static_readmember(pc+i, FE_LINUXPERF_FD_MAJOR_PAGE_FAULTS                , &res[i].sw.major_page_faults                );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1D_READ_ACCESSES                , &res[i].cache.l1d.read.accesses             );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1D_WRITE_ACCESSES               , &res[i].cache.l1d.write.accesses            );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1D_PREFETCH_ACCESSES            , &res[i].cache.l1d.prefetch.accesses         );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1D_READ_MISSES                  , &res[i].cache.l1d.read.misses               );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1D_WRITE_MISSES                 , &res[i].cache.l1d.write.misses              );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1D_PREFETCH_MISSES              , &res[i].cache.l1d.prefetch.misses           );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1I_READ_ACCESSES                , &res[i].cache.l1i.read.accesses             );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1I_WRITE_ACCESSES               , &res[i].cache.l1i.write.accesses            );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1I_PREFETCH_ACCESSES            , &res[i].cache.l1i.prefetch.accesses         );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1I_READ_MISSES                  , &res[i].cache.l1i.read.misses               );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1I_WRITE_MISSES                 , &res[i].cache.l1i.write.misses              );
        static_readmember(pc+i, FE_LINUXPERF_FD_L1I_PREFETCH_MISSES              , &res[i].cache.l1i.prefetch.misses           );
        static_readmember(pc+i, FE_LINUXPERF_FD_LL_READ_ACCESSES                 , &res[i].cache.ll.read.accesses              );
        static_readmember(pc+i, FE_LINUXPERF_FD_LL_WRITE_ACCESSES                , &res[i].cache.ll.write.accesses             );
        static_readmember(pc+i, FE_LINUXPERF_FD_LL_PREFETCH_ACCESSES             , &res[i].cache.ll.prefetch.accesses          );
        static_readmember(pc+i, FE_LINUXPERF_FD_LL_READ_MISSES                   , &res[i].cache.ll.read.misses                );
        static_readmember(pc+i, FE_LINUXPERF_FD_LL_WRITE_MISSES                  , &res[i].cache.ll.write.misses               );
        static_readmember(pc+i, FE_LINUXPERF_FD_LL_PREFETCH_MISSES               , &res[i].cache.ll.prefetch.misses            );
        static_readmember(pc+i, FE_LINUXPERF_FD_DTLB_READ_ACCESSES               , &res[i].cache.dtlb.read.accesses            );
        static_readmember(pc+i, FE_LINUXPERF_FD_DTLB_WRITE_ACCESSES              , &res[i].cache.dtlb.write.accesses           );
        static_readmember(pc+i, FE_LINUXPERF_FD_DTLB_PREFETCH_ACCESSES           , &res[i].cache.dtlb.prefetch.accesses        );
        static_readmember(pc+i, FE_LINUXPERF_FD_DTLB_READ_MISSES                 , &res[i].cache.dtlb.read.misses              );
        static_readmember(pc+i, FE_LINUXPERF_FD_DTLB_WRITE_MISSES                , &res[i].cache.dtlb.write.misses             );
        static_readmember(pc+i, FE_LINUXPERF_FD_DTLB_PREFETCH_MISSES             , &res[i].cache.dtlb.prefetch.misses          );
        static_readmember(pc+i, FE_LINUXPERF_FD_ITLB_READ_ACCESSES               , &res[i].cache.itlb.read.accesses            );
        static_readmember(pc+i, FE_LINUXPERF_FD_ITLB_WRITE_ACCESSES              , &res[i].cache.itlb.write.accesses           );
        static_readmember(pc+i, FE_LINUXPERF_FD_ITLB_PREFETCH_ACCESSES           , &res[i].cache.itlb.prefetch.accesses        );
        static_readmember(pc+i, FE_LINUXPERF_FD_ITLB_READ_MISSES                 , &res[i].cache.itlb.read.misses              );
        static_readmember(pc+i, FE_LINUXPERF_FD_ITLB_WRITE_MISSES                , &res[i].cache.itlb.write.misses             );
        static_readmember(pc+i, FE_LINUXPERF_FD_ITLB_PREFETCH_MISSES             , &res[i].cache.itlb.prefetch.misses          );
        static_readmember(pc+i, FE_LINUXPERF_FD_BPU_READ_ACCESSES                , &res[i].cache.bpu.read.accesses             );
        static_readmember(pc+i, FE_LINUXPERF_FD_BPU_WRITE_ACCESSES               , &res[i].cache.bpu.write.accesses            );
        static_readmember(pc+i, FE_LINUXPERF_FD_BPU_PREFETCH_ACCESSES            , &res[i].cache.bpu.prefetch.accesses         );
        static_readmember(pc+i, FE_LINUXPERF_FD_BPU_READ_MISSES                  , &res[i].cache.bpu.read.misses               );
        static_readmember(pc+i, FE_LINUXPERF_FD_BPU_WRITE_MISSES                 , &res[i].cache.bpu.write.misses              );
        static_readmember(pc+i, FE_LINUXPERF_FD_BPU_PREFETCH_MISSES              , &res[i].cache.bpu.prefetch.misses           );
        static_readmember(pc+i, FE_LINUXPERF_FD_NODE_READ_ACCESSES               , &res[i].cache.local_memory.read.accesses    );
        static_readmember(pc+i, FE_LINUXPERF_FD_NODE_WRITE_ACCESSES              , &res[i].cache.local_memory.write.accesses   );
        static_readmember(pc+i, FE_LINUXPERF_FD_NODE_PREFETCH_ACCESSES           , &res[i].cache.local_memory.prefetch.accesses);
        static_readmember(pc+i, FE_LINUXPERF_FD_NODE_READ_MISSES                 , &res[i].cache.local_memory.read.misses      );
        static_readmember(pc+i, FE_LINUXPERF_FD_NODE_WRITE_MISSES                , &res[i].cache.local_memory.write.misses     );
        static_readmember(pc+i, FE_LINUXPERF_FD_NODE_PREFETCH_MISSES             , &res[i].cache.local_memory.prefetch.misses  );
    }

    prctl(PR_TASK_PERF_EVENTS_ENABLE, 0, 0, 0, 0);
}
#endif /* FE_LINUXPERF_UNSUPPORTED */

void fe_linuxperf_state_visit(fe_linuxperf_state *res, void(*visit)(const char *name, fe_linuxperf_state_counter *val)) {
#define HELPER(member) visit(#member, &(res->member))
    HELPER(hw.total_cycles                     );
    HELPER(hw.instructions                     );
    HELPER(hw.cache_accesses                   );
    HELPER(hw.cache_misses                     );
    HELPER(hw.branch_instructions              );
    HELPER(hw.branch_mispredictions            );
    HELPER(hw.bus_cycles                       );
    HELPER(hw.stalled_cycles_frontend          );
    HELPER(hw.stalled_cycles_backend           );
    HELPER(hw.total_cycles_no_frequency_scaling);
    HELPER(sw.page_faults                      );
    HELPER(sw.context_switches                 );
    HELPER(sw.cpu_migrations                   );
    HELPER(sw.minor_page_faults                );
    HELPER(sw.major_page_faults                );
    HELPER(cache.l1d.read.accesses             );
    HELPER(cache.l1d.write.accesses            );
    HELPER(cache.l1d.prefetch.accesses         );
    HELPER(cache.l1d.read.misses               );
    HELPER(cache.l1d.write.misses              );
    HELPER(cache.l1d.prefetch.misses           );
    HELPER(cache.l1i.read.accesses             );
    HELPER(cache.l1i.write.accesses            );
    HELPER(cache.l1i.prefetch.accesses         );
    HELPER(cache.l1i.read.misses               );
    HELPER(cache.l1i.write.misses              );
    HELPER(cache.l1i.prefetch.misses           );
    HELPER(cache.ll.read.accesses              );
    HELPER(cache.ll.write.accesses             );
    HELPER(cache.ll.prefetch.accesses          );
    HELPER(cache.ll.read.misses                );
    HELPER(cache.ll.write.misses               );
    HELPER(cache.ll.prefetch.misses            );
    HELPER(cache.dtlb.read.accesses            );
    HELPER(cache.dtlb.write.accesses           );
    HELPER(cache.dtlb.prefetch.accesses        );
    HELPER(cache.dtlb.read.misses              );
    HELPER(cache.dtlb.write.misses             );
    HELPER(cache.dtlb.prefetch.misses          );
    HELPER(cache.itlb.read.accesses            );
    HELPER(cache.itlb.write.accesses           );
    HELPER(cache.itlb.prefetch.accesses        );
    HELPER(cache.itlb.read.misses              );
    HELPER(cache.itlb.write.misses             );
    HELPER(cache.itlb.prefetch.misses          );
    HELPER(cache.bpu.read.accesses             );
    HELPER(cache.bpu.write.accesses            );
    HELPER(cache.bpu.prefetch.accesses         );
    HELPER(cache.bpu.read.misses               );
    HELPER(cache.bpu.write.misses              );
    HELPER(cache.bpu.prefetch.misses           );
    HELPER(cache.local_memory.read.accesses    );
    HELPER(cache.local_memory.write.accesses   );
    HELPER(cache.local_memory.prefetch.accesses);
    HELPER(cache.local_memory.read.misses      );
    HELPER(cache.local_memory.write.misses     );
    HELPER(cache.local_memory.prefetch.misses  );
}

bool fe_linuxperf_setup(void) {
#ifdef FE_LINUXPERF_UNSUPPORTED
    return false;
#else
    if(access("/proc/sys/kernel/perf_event_paranoid", F_OK))
        return false;
    ptr_linuxperf_init = real_linuxperf_init;
    ptr_linuxperf_deinit = real_linuxperf_deinit;
    ptr_linuxperf_restart = real_linuxperf_restart;
    ptr_linuxperf_retrieve = real_linuxperf_retrieve;
    return true;
#endif
}

#endif /* FE_TARGET_LINUX */
