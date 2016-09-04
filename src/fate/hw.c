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

#if defined(FE_HW_TARGET_X86) && defined(FE_HW_HAS_MULTIMEDIA_INTRINSICS)
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
#endif
static void static_sse_prefetch_dummy(void *addr, bool rw) {}
void (*fe_hw_prefetch_t0)(void *, bool) = static_sse_prefetch_dummy;
void (*fe_hw_prefetch_t1)(void *, bool)  = static_sse_prefetch_dummy;
void (*fe_hw_prefetch_t2)(void *, bool)  = static_sse_prefetch_dummy;
void (*fe_hw_prefetch_nta)(void *, bool) = static_sse_prefetch_dummy;


static fe_hw_cacheinfo_struct static_cacheinfo;
const fe_hw_cacheinfo_struct *const fe_hw_cacheinfo = &static_cacheinfo;

#if defined(FE_TARGET_LINUX) && !defined(FE_TARGET_ANDROID)
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



#ifdef FE_TARGET_ANDROID
size_t fe_hw_get_cpu_count(void) {
    return android_getCpuCount();
}
#elif defined(FE_TARGET_LINUX)
size_t fe_hw_get_cpu_count(void) {
    long conf = sysconf(_SC_NPROCESSORS_ONLN);
    return conf*(conf>0);
}
#else
FE_NIY size_t fe_hw_get_cpu_count(void) {
    return 0;
}
#endif


#ifdef FE_HW_TARGET_X86

    const fe_hw_x86_features_struct fe_hw_x86_cpu_info = {0};

    #if (defined(__GNUC__) || defined(__clang__))
        #include <cpuid.h>
    #endif

    void fe_hw_x86_cpuidex(uint32_t leaf, uint32_t subleaf, 
                           uint32_t *eax, uint32_t *ebx, 
                           uint32_t *ecx, uint32_t *edx) {
    #if (defined(__GNUC__) || defined(__clang__))
        __cpuid_count(leaf, subleaf, eax, ebx, ecx, edx);
    #elif defined(_MSC_VER)
        int regs[4];
        __cpuidex(regs, leaf, subleaf);
        *eax = regs[0];
        *ebx = regs[1];
        *ecx = regs[2];
        *edx = regs[3];
    #else
    #error "No cpuid intrinsic for this target !"
    #endif
    }

    static void static_x86_cpuid_fill(void) {
        fe_hw_x86_features_struct *cpuinfo = (void*)&fe_hw_x86_cpu_info;
        uint32_t a=0, b=0, c=0, d=0;
        uint32_t *vid = (void*)cpuinfo->vendor_id;
        uint32_t basic_max=0, ext_max=0;

        fe_hw_x86_cpuidex(0x00000000, 0, &basic_max, vid, vid+1, vid+2);
        fe_hw_x86_cpuidex(0x80000000, 0, &ext_max, &b, &c, &d);

        if(basic_max < 1)
            return;

        fe_hw_x86_cpuidex(1, 0, &a, &b, &c, &d);

        cpuinfo->has_cmpxchg8b   = !!(d & FE_HW_X86_CPUID_BIT_CMPXCHG8B  );
        cpuinfo->has_cmov        = !!(d & FE_HW_X86_CPUID_BIT_CMOV       ); 
        cpuinfo->has_mmx         = !!(d & FE_HW_X86_CPUID_BIT_MMX        );
        cpuinfo->has_fxsave      = !!(d & FE_HW_X86_CPUID_BIT_FXSAVE     );
        cpuinfo->has_sse         = !!(d & FE_HW_X86_CPUID_BIT_SSE        );
        cpuinfo->has_sse2        = !!(d & FE_HW_X86_CPUID_BIT_SSE2       );

        cpuinfo->has_sse3        = !!(c & FE_HW_X86_CPUID_BIT_SSE3       );
        cpuinfo->has_pclmul      = !!(c & FE_HW_X86_CPUID_BIT_PCLMUL     );
        cpuinfo->has_lzcnt       = !!(c & FE_HW_X86_CPUID_BIT_LZCNT      );
        cpuinfo->has_ssse3       = !!(c & FE_HW_X86_CPUID_BIT_SSSE3      );
        cpuinfo->has_fma         = !!(c & FE_HW_X86_CPUID_BIT_FMA        );
        cpuinfo->has_cmpxchg16b  = !!(c & FE_HW_X86_CPUID_BIT_CMPXCHG16B );
        cpuinfo->has_sse4_1      = !!(c & FE_HW_X86_CPUID_BIT_SSE4_1     );
        cpuinfo->has_sse4_2      = !!(c & FE_HW_X86_CPUID_BIT_SSE4_2     );
        cpuinfo->has_movbe       = !!(c & FE_HW_X86_CPUID_BIT_MOVBE      );
        cpuinfo->has_popcnt      = !!(c & FE_HW_X86_CPUID_BIT_POPCNT     );
        cpuinfo->has_aes         = !!(c & FE_HW_X86_CPUID_BIT_AES        );
        cpuinfo->has_xsave       = !!(c & FE_HW_X86_CPUID_BIT_XSAVE      );
        cpuinfo->has_osxsave     = !!(c & FE_HW_X86_CPUID_BIT_OSXSAVE    );
        cpuinfo->has_avx         = !!(c & FE_HW_X86_CPUID_BIT_AVX        );
        cpuinfo->has_f16c        = !!(c & FE_HW_X86_CPUID_BIT_F16C       );
        cpuinfo->has_rdrnd       = !!(c & FE_HW_X86_CPUID_BIT_RDRND      );

        if(basic_max < 7)
            return;

        fe_hw_x86_cpuidex(7, 0, &a, &b, &c, &d);

        cpuinfo->has_fsgsbase    = !!(b & FE_HW_X86_CPUID_BIT_FSGSBASE   );
        cpuinfo->has_bmi         = !!(b & FE_HW_X86_CPUID_BIT_BMI        );
        cpuinfo->has_hle         = !!(b & FE_HW_X86_CPUID_BIT_HLE        );
        cpuinfo->has_avx2        = !!(b & FE_HW_X86_CPUID_BIT_AVX2       );
        cpuinfo->has_bmi2        = !!(b & FE_HW_X86_CPUID_BIT_BMI2       );
        cpuinfo->has_rtm         = !!(b & FE_HW_X86_CPUID_BIT_RTM        );
        cpuinfo->has_mpx         = !!(b & FE_HW_X86_CPUID_BIT_MPX        );
        cpuinfo->has_avx512f     = !!(b & FE_HW_X86_CPUID_BIT_AVX512F    );
        cpuinfo->has_avx512dq    = !!(b & FE_HW_X86_CPUID_BIT_AVX512DQ   );
        cpuinfo->has_rdseed      = !!(b & FE_HW_X86_CPUID_BIT_RDSEED     );
        cpuinfo->has_adx         = !!(b & FE_HW_X86_CPUID_BIT_ADX        );
        cpuinfo->has_avx512ifma  = !!(b & FE_HW_X86_CPUID_BIT_AVX512IFMA );
        cpuinfo->has_pcommit     = !!(b & FE_HW_X86_CPUID_BIT_PCOMMIT    );
        cpuinfo->has_clflushopt  = !!(b & FE_HW_X86_CPUID_BIT_CLFLUSHOPT );
        cpuinfo->has_clwb        = !!(b & FE_HW_X86_CPUID_BIT_CLWB       );
        cpuinfo->has_avx512pf    = !!(b & FE_HW_X86_CPUID_BIT_AVX512PF   );
        cpuinfo->has_avx512er    = !!(b & FE_HW_X86_CPUID_BIT_AVX512ER   );
        cpuinfo->has_avx512cd    = !!(b & FE_HW_X86_CPUID_BIT_AVX512CD   );
        cpuinfo->has_sha         = !!(b & FE_HW_X86_CPUID_BIT_SHA        );
        cpuinfo->has_avx512bw    = !!(b & FE_HW_X86_CPUID_BIT_AVX512BW   );
        cpuinfo->has_avx512vl    = !!(b & FE_HW_X86_CPUID_BIT_AVX512VL   );

        cpuinfo->has_prefetchwt1 = !!(c & FE_HW_X86_CPUID_BIT_PREFETCHWT1);
        cpuinfo->has_avx512vbmi  = !!(c & FE_HW_X86_CPUID_BIT_AVX512VBMI );
        cpuinfo->has_pku         = !!(c & FE_HW_X86_CPUID_BIT_PKU        );
        cpuinfo->has_ospke       = !!(c & FE_HW_X86_CPUID_BIT_OSPKE      );

        if(ext_max < 0x80000001)
            return;

        fe_hw_x86_cpuidex(0x80000001, 0, &a, &b, &c, &d);

        cpuinfo->has_lahf_lm     = !!(c & FE_HW_X86_CPUID_BIT_LAHF_LM    );
        cpuinfo->has_abm         = !!(c & FE_HW_X86_CPUID_BIT_ABM        );
        cpuinfo->has_sse4a       = !!(c & FE_HW_X86_CPUID_BIT_SSE4a      );
        cpuinfo->has_prfchw      = !!(c & FE_HW_X86_CPUID_BIT_PRFCHW     );
        cpuinfo->has_xop         = !!(c & FE_HW_X86_CPUID_BIT_XOP        );
        cpuinfo->has_lwp         = !!(c & FE_HW_X86_CPUID_BIT_LWP        );
        cpuinfo->has_fma4        = !!(c & FE_HW_X86_CPUID_BIT_FMA4       );
        cpuinfo->has_tbm         = !!(c & FE_HW_X86_CPUID_BIT_TBM        );
        cpuinfo->has_mwaitx      = !!(c & FE_HW_X86_CPUID_BIT_MWAITX     );

        cpuinfo->has_mmxext      = !!(d & FE_HW_X86_CPUID_BIT_MMXEXT     );
        cpuinfo->has_lm          = !!(d & FE_HW_X86_CPUID_BIT_LM         );
        cpuinfo->has_3dnowext    = !!(d & FE_HW_X86_CPUID_BIT_3DNOWP     );
        cpuinfo->has_3dnow       = !!(d & FE_HW_X86_CPUID_BIT_3DNOW      );
    }

#endif

void fe_hw_setup(void) {
    cacheinfo_fill(&static_cacheinfo);
#ifdef FE_HW_TARGET_X86
    static_x86_cpuid_fill();
#ifdef FE_HW_HAS_MULTIMEDIA_INTRINSICS
    if(fe_hw_x86_cpu_info.has_sse) {
        fe_hw_prefetch_t0  = static_sse_prefetch_t0;
        fe_hw_prefetch_t1  = static_sse_prefetch_t1;
        fe_hw_prefetch_t2  = static_sse_prefetch_t2;
        fe_hw_prefetch_nta = static_sse_prefetch_nta;
    }
    if(fe_hw_x86_cpu_info.has_sse2) {
        fe_hw_clflush = static_clflush;
        fe_hw_mm_pause = static_mm_pause;
    }
#endif
#endif
}

