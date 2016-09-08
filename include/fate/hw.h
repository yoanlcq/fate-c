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

/*! \file fate/hw.h
 *  \brief Hardware-specific, Assembly, intrinsics, etc.
 *  \defgroup hw Hardware-specific, Assembly, intrinsics, etc.
 *
 * TODO
 *
 * @{
 */


#ifndef FE_HW_H
#define FE_HW_H

#include <fate/defs.h>
#include <stdbool.h>
#include <stdint.h>

/*! \brief Call once before using anything in the fe_hw API. */
void fe_hw_setup(void);


/* See http://nadeausoftware.com/articles/2012/02/c_c_tip_how_detect_processor_type_using_compiler_predefined_macros#x86andx8664 
 * See also :
 * https://sourceforge.net/p/predef/wiki/Architectures/ */

#if defined(__DOXYGEN__) \
 || defined(i386) \
 || defined(__i386) \
 || defined(__i386__) \
 || defined(_M_IX86)
/*! \brief Defined only if we're compiling for i386 hardware. */
#define FE_HW_TARGET_I386 1
#define FE_HW_TARGET_IA32 1
#define FE_HW_TARGET_X86_32 1
#define FE_HW_TARGET_X86_BITS 32
#endif

#if defined(__DOXYGEN__) \
 || defined(__x86_64) \
 || defined(__x86_64__) \
 || defined(__amd64) \
 || defined(__amd64__) \
 || defined(_M_AMD64) \
 || defined(_M_X64)
/*! \brief Defined only if we're compiling for x86-64 hardware. */
#define FE_HW_TARGET_AMD64 1
#define FE_HW_TARGET_X64 1
#define FE_HW_TARGET_X86_64 1
#define FE_HW_TARGET_X86_BITS 64
#endif

#if defined(FE_HW_TARGET_X86_32) \
 || defined(FE_HW_TARGET_X86_64) \
 || defined(_X86_)
/*! \brief Defined only if we're compiling for x86 hardware. */
#define FE_HW_TARGET_X86 1
#endif

#ifndef FE_HW_TARGET_X86_BITS
#error "Are we compiling for i386 or amd64 ??"
#endif

#if __DOXYGEN__ || defined(_M_ARM) || defined(__arm__)

/*! \brief Defined only if we're compiling for ARM. */
    #define FE_HW_TARGET_ARM

    #if __DOXYGEN__ || defined(_M_ARMT) || defined(__thumb__)
/*! \brief Defined only if we're compiling for ARM with Thumb.  */
        #define FE_HW_TARGET_ARM_THUMB
    #endif
    #if __DOXYGEN__ || defined(__ARM_NEON__)
/*! \brief Defined only if we're compiling for ARM with NEON instructions. */
        #define FE_HW_TARGET_ARM_NEON
    #endif
    #if __DOXYGEN__ || defined(__IWMMXT__)
/*! \brief Defined only if we're compiling for ARM with iWMMXt instructions. */
        #define FE_HW_TARGET_ARM_IWMMXT
    #endif

#endif

#if __DOXYGEN__ || defined(__aarch64__)
/*! \brief Defined only if we're compiling for ARM64. */
#define FE_HW_TARGET_ARM64
#endif

#if defined(FE_HW_TARGET_ARM) && !defined(FE_HW_TARGET_ARM64)
/*! \brief Defined only if we're compiling for ARM32. */
#define FE_HW_TARGET_ARM32
#endif


#if __DOXYGEN__ \
 || defined(_MIPS_ARCH_MIPS32) \
 || defined(_MIPS_ARCH_MIPS32R2) \
 || defined(_MIPS_ARCH_MIPS32R3) \
 || defined(_MIPS_ARCH_MIPS32R5) \
 || defined(_MIPS_ARCH_MIPS32R6)
/*! \brief Defined only if we're compiling for MIPS32. */
#define FE_HW_TARGET_MIPS32
#endif

#if __DOXYGEN__ \
 || defined(_MIPS_ARCH_MIPS64) \
 || defined(_MIPS_ARCH_MIPS64R2) \
 || defined(_MIPS_ARCH_MIPS64R3) \
 || defined(_MIPS_ARCH_MIPS64R5) \
 || defined(_MIPS_ARCH_MIPS64R6)
/*! \brief Defined only if we're compiling for MIPS64. */
#define FE_HW_TARGET_MIPS64
#endif


#if defined(FE_HW_TARGET_MIPS32) || defined(FE_HW_TARGET_MIPS64)
/*! \brief Defined only if we're compiling for MIPS. */
#define FE_HW_TARGET_MIPS
#endif


/*! \brief Defined only if compiler intrinsics are provided. */
#define FE_HW_HAS_MULTIMEDIA_INTRINSICS

/* Shamelessly taken from http://stackoverflow.com/a/22291538 */
#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) && defined(FE_HW_TARGET_X86)
    #include <x86intrin.h>
#elif defined(__GNUC__) && defined(FE_HW_TARGET_ARM_NEON)
    #include <arm_neon.h>
#elif defined(__GNUC__) && defined(FE_HW_TARGET_ARM_WMMX)
    #include <mmintrin.h>
#else
    #undef FE_HW_HAS_MULTIMEDIA_INTRINSICS
#endif


#ifdef FE_TARGET_ANDROID
/* See https://developer.android.com/ndk/guides/cpu-features.html */
#include <cpu-features.h>
#endif



#if __DOXYGEN__ || defined(__GNUC__)
/*! \brief Prefetch data, hinting that it should be placed into the 
 *         appropriate CPU cache level.
 *
 * There's a lot to learn before using manual prefetching properly, but here
 * are the basics :
 * - Your CPU is pretty good at doing its job. Even when you believe you're
 *   providing it with valid hints, performance might just drop a bit instead;
 * - Just like the multimedia intrinsics, it's only interesting when processing
 *   large amounts of data;
 * - Use it very early, so that by the time you actually need the data, 
 *   it is there in the cache.
 * - Profile. That's it.
 *
 * Currently, the first of these that is supported is used 
 * as the implementation :
 * - \c __builtin_prefetch() (GCC)
 * - \c _mm_prefetch() (Multimedia intrinsics)
 * - Nothing, and this macro expands to nothing.
 *
 * \param addr The data's address.
 * \param rw Compile-time boolean : "Will the data be written to ?"
 */
#define fe_hw_prefetch_t0(addr,rw) __builtin_prefetch(addr,rw,3)
/*! \brief Prefetch data to the L2 CPU cache. See the description at 
 *         #fe_hw_prefetch_t0. */
#define fe_hw_prefetch_t1(addr,rw)  __builtin_prefetch(addr,rw,2)
/*! \brief Prefetch data to the L3 CPU cache. See the description at 
 *         #fe_hw_prefetch_t0. */
#define fe_hw_prefetch_t2(addr,rw)  __builtin_prefetch(addr,rw,1)
/*! \brief Prefetch data with Non-Temporal Aligned hint. See the description at 
 *         #fe_hw_prefetch_t0. 
 *
 * Basically, prefer this when the data is not to be used for long.
 */
#define fe_hw_prefetch_nta(addr,rw) __builtin_prefetch(addr,rw,0)
/*! \brief Provides branch prediction information to the compiler. 
 *
 * This macro expands to \p cond if the compiler does not support 
 * \c __builtin_expect().
 *
 * Use it when you expect \c cond to be very often non-zero.
 *
 * Please be careful using this macro. 
 * \c __builtin_expect() has been reported to provide only small
 * performance boosts when used correctly, but potentially huge performance
 * drops otherwise.
 */
#define fe_hw_likely(cond)   __builtin_expect(!!(cond),1)
/*! \brief Provides branch prediction information to the compiler. 
 * This macro expands to \p cond if the compiler does not support 
 * \c __builtin_expect().
 *
 * Use it when you expect \c cond to be very often zero.
 *
 * Please be careful using this macro. 
 * \c __builtin_expect() has been reported to provide only small
 * performance boosts when used correctly, but potentially huge performance
 * drops otherwise.
 */
#define fe_hw_unlikely(cond) __builtin_expect(cond,0)

#else
/* Evaluate addr anyway, in case it has side effects. */
#define fe_hw_likely(cond) (cond)
#define fe_hw_unlikely(cond) (cond)
#endif

#if !defined(fe_hw_prefetch_nta)
#if defined(FE_HW_TARGET_X86) && defined(FE_HW_HAS_MULTIMEDIA_INTRINSICS)
extern void (*fe_hw_prefetch_t0)(void *addr, bool rw);
extern void (*fe_hw_prefetch_t1)(void *addr, bool rw);
extern void (*fe_hw_prefetch_t2)(void *addr, bool rw);
extern void (*fe_hw_prefetch_nta)(void *addr, bool rw);
#else
#define fe_hw_prefetch_t0(addr,rw)
#define fe_hw_prefetch_t1(addr,rw)
#define fe_hw_prefetch_t2(addr,rw)
#define fe_hw_prefetch_nta(addr,rw)
#endif /* FE_HW_HAS_MULTIMEDIA_INTRINSICS */
#endif /* fe_hw_prefetch */




#if defined(FE_HW_TARGET_X86)
/*! \brief Defined only if \c _mm_malloc() and \c _mm_free() are available. */
#define FE_HW_HAS_MM_MALLOC

#if defined(__INTEL_COMPILER)
#include <malloc.h>
#elif defined(__GNUC__)
#include <mm_malloc.h>
#else
#undef FE_HW_HAS_MM_MALLOC
#endif // defined(__GNUC__)

#endif /* FE_HW_TARGET_X86 */





/*! \brief Struct holding useful information about our CPU caches. */
typedef struct {
    struct {
        size_t size, ways_of_associativity, line_size;
    } l1i, l1d, l2, l3, l4;
} fe_hw_cacheinfo_struct;

/*! \brief A pointer to an initialized #fe_hw_cacheinfo_struct.
 * 
 * The underlying object is filled by #fe_hw_setup().
 */
extern fe_hw_cacheinfo_struct fe_hw_cacheinfo;

typedef struct {
    char vendor_id[13];
    bool has_cmpxchg8b   : 1;
    bool has_cmov        : 1;
    bool has_mmx         : 1;
    bool has_fxsave      : 1;
    bool has_sse         : 1;
    bool has_sse2        : 1;

    bool has_sse3        : 1;
    bool has_pclmul      : 1;
    bool has_lzcnt       : 1;
    bool has_ssse3       : 1;
    bool has_fma         : 1;
    bool has_cmpxchg16b  : 1;
    bool has_sse4_1      : 1;
    bool has_sse4_2      : 1;
    bool has_movbe       : 1;
    bool has_popcnt      : 1;
    bool has_aes         : 1;
    bool has_xsave       : 1;
    bool has_osxsave     : 1;
    bool has_avx         : 1;
    bool has_f16c        : 1;
    bool has_rdrnd       : 1;

    bool has_fsgsbase    : 1;
    bool has_bmi         : 1;
    bool has_hle         : 1;
    bool has_avx2        : 1;
    bool has_bmi2        : 1;
    bool has_rtm         : 1;
    bool has_mpx         : 1;
    bool has_avx512f     : 1;
    bool has_avx512dq    : 1;
    bool has_rdseed      : 1;
    bool has_adx         : 1;
    bool has_avx512ifma  : 1;
    bool has_pcommit     : 1;
    bool has_clflushopt  : 1;
    bool has_clwb        : 1;
    bool has_avx512pf    : 1;
    bool has_avx512er    : 1;
    bool has_avx512cd    : 1;
    bool has_sha         : 1;
    bool has_avx512bw    : 1;
    bool has_avx512vl    : 1;

    bool has_prefetchwt1 : 1;
    bool has_avx512vbmi  : 1;
    bool has_pku         : 1;
    bool has_ospke       : 1;

    bool has_lahf_lm     : 1;
    bool has_abm         : 1;
    bool has_sse4a       : 1;
    bool has_prfchw      : 1;
    bool has_xop         : 1;
    bool has_lwp         : 1;
    bool has_fma4        : 1;
    bool has_tbm         : 1;
    bool has_mwaitx      : 1;

    bool has_mmxext      : 1;
    bool has_lm          : 1;
    bool has_3dnowext    : 1;
    bool has_3dnow       : 1;
} fe_hw_x86_features_struct;

#if __DOXYGEN__ || FE_HW_TARGET_X86

    /*! \brief Check your x86 CPU's features. The data is filled by the
    * CPUID instruction at the time of the call to #fe_hw_setup(). */
    extern fe_hw_x86_features_struct fe_hw_x86_cpu_info;

    /*! \brief TODO */
    void fe_hw_x86_cpuidex(uint32_t leaf, uint32_t subleaf, 
                           uint32_t *eax, uint32_t *ebx, 
                           uint32_t *ecx, uint32_t *edx);

    /* Following definitons taken from GCC's cpuid.h. */

    #define FE_HW_X86_CPUID_BIT_SSE3    (1 << 0)
    #define FE_HW_X86_CPUID_BIT_PCLMUL  (1 << 1)
    #define FE_HW_X86_CPUID_BIT_LZCNT   (1 << 5)
    #define FE_HW_X86_CPUID_BIT_SSSE3   (1 << 9)
    #define FE_HW_X86_CPUID_BIT_FMA     (1 << 12)
    #define FE_HW_X86_CPUID_BIT_CMPXCHG16B  (1 << 13)
    #define FE_HW_X86_CPUID_BIT_SSE4_1  (1 << 19)
    #define FE_HW_X86_CPUID_BIT_SSE4_2  (1 << 20)
    #define FE_HW_X86_CPUID_BIT_MOVBE   (1 << 22)
    #define FE_HW_X86_CPUID_BIT_POPCNT  (1 << 23)
    #define FE_HW_X86_CPUID_BIT_AES     (1 << 25)
    #define FE_HW_X86_CPUID_BIT_XSAVE   (1 << 26)
    #define FE_HW_X86_CPUID_BIT_OSXSAVE (1 << 27)
    #define FE_HW_X86_CPUID_BIT_AVX     (1 << 28)
    #define FE_HW_X86_CPUID_BIT_F16C    (1 << 29)
    #define FE_HW_X86_CPUID_BIT_RDRND   (1 << 30)

    #define FE_HW_X86_CPUID_BIT_CMPXCHG8B   (1 << 8)
    #define FE_HW_X86_CPUID_BIT_CMOV    (1 << 15)
    #define FE_HW_X86_CPUID_BIT_MMX     (1 << 23)
    #define FE_HW_X86_CPUID_BIT_FXSAVE  (1 << 24)
    #define FE_HW_X86_CPUID_BIT_SSE     (1 << 25)
    #define FE_HW_X86_CPUID_BIT_SSE2    (1 << 26)

    #define FE_HW_X86_CPUID_BIT_LAHF_LM (1 << 0)
    #define FE_HW_X86_CPUID_BIT_ABM     (1 << 5)
    #define FE_HW_X86_CPUID_BIT_SSE4a   (1 << 6)
    #define FE_HW_X86_CPUID_BIT_PRFCHW  (1 << 8)
    #define FE_HW_X86_CPUID_BIT_XOP         (1 << 11)
    #define FE_HW_X86_CPUID_BIT_LWP     (1 << 15)
    #define FE_HW_X86_CPUID_BIT_FMA4        (1 << 16)
    #define FE_HW_X86_CPUID_BIT_TBM         (1 << 21)
    #define FE_HW_X86_CPUID_BIT_MWAITX      (1 << 29)

    #define FE_HW_X86_CPUID_BIT_MMXEXT  (1 << 22)
    #define FE_HW_X86_CPUID_BIT_LM      (1 << 29)
    #define FE_HW_X86_CPUID_BIT_3DNOWP  (1 << 30)
    #define FE_HW_X86_CPUID_BIT_3DNOW   (1 << 31)

    #define FE_HW_X86_CPUID_BIT_CLZERO  (1 << 0)

    #define FE_HW_X86_CPUID_BIT_FSGSBASE    (1 << 0)
    #define FE_HW_X86_CPUID_BIT_BMI (1 << 3)
    #define FE_HW_X86_CPUID_BIT_HLE (1 << 4)
    #define FE_HW_X86_CPUID_BIT_AVX2    (1 << 5)
    #define FE_HW_X86_CPUID_BIT_BMI2    (1 << 8)
    #define FE_HW_X86_CPUID_BIT_RTM (1 << 11)
    #define FE_HW_X86_CPUID_BIT_MPX (1 << 14)
    #define FE_HW_X86_CPUID_BIT_AVX512F (1 << 16)
    #define FE_HW_X86_CPUID_BIT_AVX512DQ    (1 << 17)
    #define FE_HW_X86_CPUID_BIT_RDSEED  (1 << 18)
    #define FE_HW_X86_CPUID_BIT_ADX (1 << 19)
    #define FE_HW_X86_CPUID_BIT_AVX512IFMA  (1 << 21)
    #define FE_HW_X86_CPUID_BIT_PCOMMIT (1 << 22)
    #define FE_HW_X86_CPUID_BIT_CLFLUSHOPT  (1 << 23)
    #define FE_HW_X86_CPUID_BIT_CLWB    (1 << 24)
    #define FE_HW_X86_CPUID_BIT_AVX512PF    (1 << 26)
    #define FE_HW_X86_CPUID_BIT_AVX512ER    (1 << 27)
    #define FE_HW_X86_CPUID_BIT_AVX512CD    (1 << 28)
    #define FE_HW_X86_CPUID_BIT_SHA     (1 << 29)
    #define FE_HW_X86_CPUID_BIT_AVX512BW    (1 << 30)
    #define FE_HW_X86_CPUID_BIT_AVX512VL    (1 << 31)

    #define FE_HW_X86_CPUID_BIT_PREFETCHWT1   (1 << 0)
    #define FE_HW_X86_CPUID_BIT_AVX512VBMI  (1 << 1)
    #define FE_HW_X86_CPUID_BIT_PKU (1 << 3)
    #define FE_HW_X86_CPUID_BIT_OSPKE   (1 << 4)

#endif



typedef struct {
    uint32_t cpuid;
    bool has_armv7       : 1; 
    bool has_vfpv3       : 1; 
    bool has_neon        : 1; 
    bool has_ldrex_strex : 1; 
    bool has_vfpv2       : 1; 
    bool has_vfp_d32     : 1; 
    bool has_vfp_fp16    : 1; 
    bool has_vfp_fma     : 1; 
    bool has_neon_fma    : 1; 
    bool has_idiv_arm    : 1; 
    bool has_idiv_thumb2 : 1; 
    bool has_iwmmxt      : 1; 
    bool has_aes         : 1; 
    bool has_pmull       : 1; 
    bool has_sha1        : 1; 
    bool has_sha2        : 1; 
    bool has_crc32       : 1;
} fe_hw_arm32_features_struct;

typedef struct {
    uint32_t cpuid;
    bool has_fp    : 1;
    bool has_asimd : 1;
    bool has_aes   : 1;
    bool has_pmull : 1;
    bool has_sha1  : 1;
    bool has_sha2  : 1;
    bool has_crc32 : 1;
} fe_hw_arm64_features_struct;


#if __DOXYGEN__ || defined(FE_HW_TARGET_ARM32)
/*! \brief Check your ARM CPU's features. The data is filled
    *  at the time of the call to #fe_hw_setup(). */
    extern fe_hw_arm32_features_struct fe_hw_arm32_cpu_info;
#endif
#if __DOXYGEN__ || defined(FE_HW_TARGET_ARM64)
/*! \brief Check your ARM64 CPU's features. The data is filled
    *  at the time of the call to #fe_hw_setup(). */
    extern fe_hw_arm64_features_struct fe_hw_arm64_cpu_info;
#endif



#if __DOXYGEN__ || defined(FE_HW_TARGET_X86)
/*! \brief Invalidates the cache line containing \p addr. */
extern void (*fe_hw_clflush)(void const *addr);
#else
#define fe_hw_clflush(addr) 
#endif

#if __DOXYGEN__  \
 || ( defined(FE_HW_TARGET_X86) && defined(FE_HW_HAS_MULTIMEDIA_INTRINSICS) \
     && (defined(_MSC_VER) || defined(__GNUC__)) )
/*! \brief Defined only if #fe_hw_rdtsc() is available. */
#define FE_HW_HAS_RDTSC
/*! \brief "Read Timestamp Counter" instruction wrapper. 
 *
 * \return Value of type uint64_t.
 */
#define fe_hw_rdtsc() __rdtsc()
#endif

#if __DOXYGEN__ || defined(FE_HW_HAS_MULTIMEDIA_INTRINSICS)
/*! \brief Hints that we are in a spin-wait loop.
 *
 * From the Intel Intrinsics Guide:\n
 * <tt>
 * Provide a hint to the processor that the code 
 * sequence is a spin-wait loop. 
 * This can help improve the performance and 
 * power consumption of spin-wait loops.
 * </tt>
 */
extern void (*fe_hw_mm_pause)(void);
#else
#define fe_hw_mm_pause()
#endif

/*! \brief TODO */
size_t fe_hw_get_cpu_count(void);

#include <SDL2/SDL.h>
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define fe_hw_swap16_host_to_net(x) SDL_Swap16(x)
#define fe_hw_swap32_host_to_net(x) SDL_Swap32(x)
#define fe_hw_swap64_host_to_net(x) SDL_Swap64(x)
#define fe_hw_swapflt_host_to_net(x) SDL_SwapFloat(x)
#define fe_hw_swap16_net_to_host(x) SDL_SwapBE16(x)
#define fe_hw_swap32_net_to_host(x) SDL_SwapBE32(x)
#define fe_hw_swap64_net_to_host(x) SDL_SwapBE64(x)
#define fe_hw_swapflt_net_to_host(x) SDL_SwapFloatBE(x)
#define fe_hw_swap16_host_to_le(x) (x)
#define fe_hw_swap32_host_to_le(x) (x)
#define fe_hw_swap64_host_to_le(x) (x)
#define fe_hw_swapflt_host_to_le(x) (x)
#define fe_hw_swap16_le_to_host(x) (x)
#define fe_hw_swap32_le_to_host(x) (x)
#define fe_hw_swap64_le_to_host(x) (x)
#define fe_hw_swapflt_le_to_host(x) (x)
#else
#define fe_hw_swap16_host_to_net(x) (x)
#define fe_hw_swap32_host_to_net(x) (x)
#define fe_hw_swap64_host_to_net(x) (x)
#define fe_hw_swapflt_host_to_net(x) (x)
#define fe_hw_swap16_net_to_host(x) (x)
#define fe_hw_swap32_net_to_host(x) (x)
#define fe_hw_swap64_net_to_host(x) (x)
#define fe_hw_swapflt_net_to_host(x) (x)
#define fe_hw_swap16_host_to_le(x) SDL_Swap16(x)
#define fe_hw_swap32_host_to_le(x) SDL_Swap32(x)
#define fe_hw_swap64_host_to_le(x) SDL_Swap64(x)
#define fe_hw_swapflt_host_to_le(x) SDL_SwapFloat(x)
#define fe_hw_swap16_le_to_host(x) SDL_SwapLE16(x)
#define fe_hw_swap32_le_to_host(x) SDL_SwapLE32(x)
#define fe_hw_swap64_le_to_host(x) SDL_SwapLE64(x)
#define fe_hw_swapflt_le_to_host(x) SDL_SwapFloatLE(x)
#endif



/* These are typedef-ed in case we have alignment requirements. 
 * There's no unsigned counterpart because there are no intrinsics 
 * for them with MSVC. */
typedef void*   fe_hw_atomicptr;
#ifdef _MSC_VER
typedef long    fe_hw_atomic32; /* Still 32 bits, see LONG in MSDN. */
#else
typedef int32_t fe_hw_atomic32;
#endif
typedef int64_t fe_hw_atomic64;


static inline int64_t fe_hw_atomic32_load(fe_hw_atomic32 *ptr) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_load_4(ptr, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return _InterlockedAdd(ptr, 0);
#endif
}
static inline int64_t fe_hw_atomic64_load(fe_hw_atomic64 *ptr) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_load_8(ptr, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return _InterlockedAdd64(ptr, 0);
#endif
}

static inline void fe_hw_atomic32_store(fe_hw_atomic32 *ptr, int32_t val) {
#if defined(__GNUC__) || defined(__clang__)
    __atomic_store_4(ptr, val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    _InterlockedExchange(ptr, val);
#endif
}

static inline void fe_hw_atomic64_store(fe_hw_atomic64 *ptr, int64_t val) {
#if defined(__GNUC__) || defined(__clang__)
    __atomic_store_8(ptr, val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    _InterlockedExchange64(ptr, val);
#endif
}

static inline void fe_hw_atomicptr_store(fe_hw_atomicptr *ptr, void* val) {
#if defined(__GNUC__) || defined(__clang__)
    __atomic_store(ptr, &val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    _InterlockedExchangePointer(ptr, val);
#endif
}

/* if(*ptr == expected)
 *     *ptr = desired;    //return true
 * else *expected = *ptr; //return false
 * return *ptr != *expected;
 */
static inline bool fe_hw_atomic32_cmpxchg(fe_hw_atomic32 *ptr, 
                                          int32_t expected, 
                                          int32_t desired) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_compare_exchange_4(ptr, &expected, desired, 
                false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    int64_t ptr_oldval = 
        _InterlockedCompareExchange(ptr, desired, expected);
    return ptr_oldval == expected;
#endif
}
static inline bool fe_hw_atomic64_cmpxchg(fe_hw_atomic64 *ptr, 
                                          int64_t expected, 
                                          int64_t desired) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_compare_exchange_8(ptr, &expected, desired, 
                false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    int64_t ptr_oldval = 
        _InterlockedCompareExchange64(ptr, desired, expected);
    return ptr_oldval == expected;
#endif
}
static inline bool fe_hw_atomicptr_cmpxchg(fe_hw_atomicptr *ptr, 
                                           void* expected, 
                                           void* desired) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_compare_exchange(ptr, &expected, &desired, 
                false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    void* ptr_oldval = 
        _InterlockedCompareExchangePointer(ptr, desired, expected);
    return ptr_oldval == expected;
#endif
}

static inline int64_t fe_hw_atomic32_add(fe_hw_atomic32 *ptr, int32_t val) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_add_fetch(ptr, val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return _InterlockedAdd(ptr, val);
#endif
}
static inline int64_t fe_hw_atomic64_add(fe_hw_atomic64 *ptr, int64_t val) {
#if defined(__GNUC__) || defined(__clang__)
    return __atomic_add_fetch(ptr, val, __ATOMIC_SEQ_CST);
#elif defined(_MSC_VER)
    return _InterlockedAdd64(ptr, val);
#endif
}

#define fe_hw_atomic_compilerbarrier_rw() SDL_CompilerBarrier()

/*! @} */

#endif /* FE_HW_H */
