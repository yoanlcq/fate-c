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

#include <fate/defs.h>
#include <fate/hw.h>

/* All of these static functions are because _mm_prefetch() is not
 * guaranteed to be available on all CPUs, and because every compiler
 * expects _mm_prefetch()'s second parameter to be a compile-time constant
 * (which seems legit, given that the instruction opcodes are different.) */
static void static_sse_prefetch_t0(void *addr, bool rw) {
    _mm_prefetch(addr, _MM_HINT_T0);
}
static void static_sse_prefetch_t1(void *addr, bool rw) {
    _mm_prefetch(addr, _MM_HINT_T1);
}
static void static_sse_prefetch_t2(void *addr, bool rw) {
    _mm_prefetch(addr, _MM_HINT_T2);
}
static void static_sse_prefetch_nta(void *addr, bool rw) {
    _mm_prefetch(addr, _MM_HINT_NTA);
}
static void static_sse_prefetch_dummy(void *addr, bool rw) {}
void (*fe_hw_prefetch_l1d)(void *, bool) = static_sse_prefetch_dummy;
void (*fe_hw_prefetch_l2)(void *, bool)  = static_sse_prefetch_dummy;
void (*fe_hw_prefetch_l3)(void *, bool)  = static_sse_prefetch_dummy;
void (*fe_hw_prefetch_nta)(void *, bool) = static_sse_prefetch_dummy;


static fe_hw_cacheinfo_struct static_cacheinfo;
const fe_hw_cacheinfo_struct *const fe_hw_cacheinfo = &static_cacheinfo;

#ifdef FE_TARGET_LINUX
#include <unistd.h>

static void cacheinfo_fill(fe_hw_cacheinfo_struct *ci) {
#define HELPER(member, mid) \
    ci->member.size                  = sysconf(_SC_LEVEL##mid##CACHE_SIZE); \
    ci->member.ways_of_associativity = sysconf(_SC_LEVEL##mid##CACHE_ASSOC); \
    ci->member.line_size             = sysconf(_SC_LEVEL##mid##CACHE_LINESIZE);
    HELPER(l1i, 1_I);
    HELPER(l1d, 1_D);
    HELPER(l2, 2_);
    HELPER(l3, 3_);
    HELPER(l4, 4_);
#undef HELPER
}

#else

#include <string.h>
FE_NIY static void cacheinfo_fill(fe_hw_cacheinfo_struct *ci) {
    memset(ci, 0, sizeof(*ci));
}

#endif /* FE_TARGET_IS_A_UNIX */

static void static_clflush_dummy(void const *addr) {}
static void static_clflush(void const *addr) {
    _mm_clflush(addr);
}
void (*fe_hw_clflush)(void const *addr) = static_clflush_dummy;
static void static_mm_pause_dummy(void) {}
static void static_mm_pause(void) {
    _mm_pause();
}
void (*fe_hw_mm_pause)(void) = static_mm_pause_dummy;


/* TODO: Use the CPUID intrinsics to detect features 
 * that __builtin_cpu_supports() can't report. 
 * Preferably call them once in fe_hw_setup(). */
#ifdef __GNUC__
#include <cpuid.h>
/* Use __get_cpuid_max() and __get_cpuid(). */
#elif defined(_MSC_VER)
/* Use __cpuid() and __cpuidex(). */
#endif

#ifndef fe_hw_x86_supports
FE_NIY bool fe_hw_x86_supports(const char *feature) {
    return false;
}
#endif

void fe_hw_setup(void) {
    cacheinfo_fill(&static_cacheinfo);
#if defined(FE_HW_TARGET_X86) && defined(FE_HW_HAS_MULTIMEDIA_INTRINSICS)
    if(fe_hw_x86_supports(FE_HW_X86_FEATURE_SSE)) {
        fe_hw_prefetch_l1d = static_sse_prefetch_t0;
        fe_hw_prefetch_l2  = static_sse_prefetch_t1;
        fe_hw_prefetch_l3  = static_sse_prefetch_t2;
        fe_hw_prefetch_nta = static_sse_prefetch_nta;
    }
    if(fe_hw_x86_supports(FE_HW_X86_FEATURE_SSE2)) {
        fe_hw_clflush = static_clflush;
        fe_hw_mm_pause = static_mm_pause;
    }
#endif
}

