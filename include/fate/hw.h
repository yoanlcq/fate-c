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
#if __DOXYGEN__ \
 || defined(i386) \
 || defined(__i386) \
 || defined(__i386__) \
 || defined(_M_IX86) \
 || defined(_X86_) \
 || defined(__x86_64) \
 || defined(__x86_64__) \
 || defined(__amd64) \
 || defined(__amd64__) \
 || defined(_M_AMD64) \
 || defined(_M_X64)
/*! \brief Defined only if we're compiling for x86 hardware. */
#define FE_HW_TARGET_X86 1
#endif

#if __DOXYGEN__ \
 || defined(_M_ARM) \
 || defined(_M_ARMT) \
 || defined(__arm__) \
 || defined(__thumb__)
/*! \brief Defined only if we're compiling for ARM. */
    #define FE_HW_TARGET_ARM
    #if __DOXYGEN__ || defined(_M_ARMT) || defined(__thumb__)
/*! \brief Defined only if we're compiling for ARM with Thumb  */
        #define FE_HW_TARGET_ARM_THUMB
    #endif
#endif

#if __DOXYGEN__ || defined(__aarch64__)
/*! \brief Defined only if we're compiling for ARM64. */
#define FE_HW_TARGET_ARM64
#endif



/*! \brief Defined only if _mm_* intrinsics are provided. */
#define FE_HW_HAS_MULTIMEDIA_INTRINSICS

/* Shamelessly taken from http://stackoverflow.com/a/22291538 */
#if defined(_MSC_VER)
    /* Microsoft C/C++-compatible compiler */
    #include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    /* GCC-compatible compiler, targeting x86/x86-64 */
    #include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
    /* GCC-compatible compiler, targeting ARM with NEON */
    #include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
    /* GCC-compatible compiler, targeting ARM with WMMX */
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
extern const fe_hw_cacheinfo_struct *const fe_hw_cacheinfo;


#if __DOXYGEN__ || (defined(FE_HW_TARGET_X86) && defined(__GNUC__) && !defined(__clang__))
#define FE_HW_X86_FEATURE_CMOV   "cmov"
#define FE_HW_X86_FEATURE_MMX    "mmx"
#define FE_HW_X86_FEATURE_POPCNT "popcnt"
#define FE_HW_X86_FEATURE_SSE    "sse"
#define FE_HW_X86_FEATURE_SSE2   "sse2"
#define FE_HW_X86_FEATURE_SSE3   "sse3"
#define FE_HW_X86_FEATURE_SSSE3  "ssse3"
#define FE_HW_X86_FEATURE_SSE4_1 "sse4.1"
#define FE_HW_X86_FEATURE_SSE4_2 "sse4.2"
#define FE_HW_X86_FEATURE_AVX    "avx"
#define FE_HW_X86_FEATURE_AVX2   "avx2"
/*! \brief Test our x86 CPU features.
 *
 * Currently, this is a wrapper around GCC's __builtin_cpu_supports().
 * This is a macro that expands to \c false if we're not compiling
 * for x86.
 *
 * Might be extended later.
 *
 * \param feature One of the FE_HW_X86_FEATURE_* constants.
 */
#define fe_hw_x86_supports(feature) __builtin_cpu_supports(feature)
#else
#define FE_HW_X86_FEATURE_CMOV   NULL
#define FE_HW_X86_FEATURE_MMX    NULL
#define FE_HW_X86_FEATURE_POPCNT NULL
#define FE_HW_X86_FEATURE_SSE    NULL
#define FE_HW_X86_FEATURE_SSE2   NULL
#define FE_HW_X86_FEATURE_SSE3   NULL
#define FE_HW_X86_FEATURE_SSSE3  NULL
#define FE_HW_X86_FEATURE_SSE4_1 NULL
#define FE_HW_X86_FEATURE_SSE4_2 NULL
#define FE_HW_X86_FEATURE_AVX    NULL
#define FE_HW_X86_FEATURE_AVX2   NULL
bool fe_hw_x86_supports(const char *feature);
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


/*! @} */

#endif /* FE_HW_H */
