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
#include <fate/dbg.h>
#include <fate/mem.h>
#include <fate/hw.h>

fe_hw_cacheinfo_struct fe_hw_cacheinfo;

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

#elif defined(FE_TARGET_WINDOWS)
#include <windows.h>
static const char *TAG = "fe_hw";
typedef BOOL (WINAPI *LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
FE_DECL_WIP static void cacheinfo_fill(fe_hw_cacheinfo_struct *ci) {
    memset(ci, 0, sizeof(*ci));

    LPFN_GLPI glpi = (LPFN_GLPI) GetProcAddress(
        GetModuleHandleW(TEXT("kernel32")), "GetLogicalProcessorInformation"
    );
    if(!glpi) {
        fe_logw(TAG, "GetLogicalProcessorInformation is not supported.\n");
        return;
    }

    DWORD size = 0;
    BOOL success = glpi(NULL, &size);
    DWORD err = GetLastError();
    if(!success) {
        if(err != ERROR_INSUFFICIENT_BUFFER) {
            //TODO:support system error codes logging.
            fe_logw(TAG, "GetLogicalProcessorInformation "
                    "failed with error %d\n", (int)GetLastError());
            return;
        }
    }

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION *pi = fe_mem_heapcalloc(size, uint8_t, "");
    success = glpi(pi, &size);
    if(!success) {
        fe_logw(TAG, "GetLogicalProcessorInformation "
            "failed with error %d\n", (int)GetLastError());
        return;
    }
    size_t i;
    for(i=0 ; i<size/sizeof(*pi) ; ++i) {
        (void)pi[i].ProcessorMask;
        switch(pi[i].Relationship) {
        case RelationProcessorCore:    
            (void)pi[i].ProcessorCore.Flags;
            break;
        case RelationNumaNode:         
            (void)pi[i].NumaNode.NodeNumber;
            break;
        case RelationCache:            
            (void)pi[i].Cache.Level; //1,2,3;
            (void)pi[i].Cache.Associativity; //CACHE_FULLY_ASSOCIATIVE ;
            (void)pi[i].Cache.LineSize;
            (void)pi[i].Cache.Size;
            switch(pi[i].Cache.Type) {
            case CacheUnified:     break;
            case CacheInstruction: break;
            case CacheData:        break;
            case CacheTrace:       break;
            }
            break;
        case RelationProcessorPackage: break;
        case RelationGroup:            break;
        case RelationAll: /* For -Wswitch */break;
        }
    }

    fe_mem_heapfree(pi);
}
#else
#include <string.h>
FE_DECL_NIY static void cacheinfo_fill(fe_hw_cacheinfo_struct *ci) {
    memset(ci, 0, sizeof(*ci));
}

#endif /* FE_TARGET_IS_A_UNIX */


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
size_t fe_hw_get_cpu_count(void) {
    return SDL_GetCPUCount();
}
#endif


#ifdef FE_HW_TARGET_X86

    /* Note to self : I've included cpuid.h in this file only, because
     * on Mingw-w64, it conflicts with intrin.h. Long story short, it 
     * doesn't play well with SDL2 and breaks the build.
     * Don't include <cpuid.h> in any file that would be included by others,
     * at least on Mingw-w64.
     */
    #if (defined(__GNUC__) || defined(__clang__))
        #include <cpuid.h>
    #endif

    fe_hw_x86_features_struct fe_hw_x86_cpu_info = {{0}};

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

#ifdef FE_HW_TARGET_ARM


    #ifdef FE_HW_TARGET_ARM32
    fe_hw_arm32_features_struct fe_hw_arm32_cpu_info = {0};
    #else
    fe_hw_arm64_features_struct fe_hw_arm64_cpu_info = {0};
    #endif

    static void static_arm_features_fill(void) {
        #ifdef FE_HW_TARGET_ARM32
            fe_hw_arm32_features_struct *arm32 = (void*)&fe_hw_arm32_cpu_info;
        #else
            fe_hw_arm64_features_struct *arm64 = (void*)&fe_hw_arm64_cpu_info;
        #endif
        #ifdef FE_TARGET_ANDROID
            AndroidCpuFamily family = android_getCpuFamily();
            uint64_t feats          = android_getCpuFeatures();

            #ifdef FE_HW_TARGET_ARM32

                fe_dbg_assert(family == ANDROID_CPU_FAMILY_ARM);
                arm32->cpuid = android_getCpuIdArm();
                #define HELPER(CST,cst) \
    if(feats & ANDROID_CPU_ARM_FEATURE_##CST) arm32->has_##cst = true
                HELPER(ARMv7      , armv7      ); 
                HELPER(VFPv3      , vfpv3      ); 
                HELPER(NEON       , neon       ); 
                HELPER(LDREX_STREX, ldrex_strex); 
                HELPER(VFPv2      , vfpv2      ); 
                HELPER(VFP_D32    , vfp_d32    ); 
                HELPER(VFP_FP16   , vfp_fp16   ); 
                HELPER(VFP_FMA    , vfp_fma    ); 
                HELPER(NEON_FMA   , neon_fma   ); 
                HELPER(IDIV_ARM   , idiv_arm   ); 
                HELPER(IDIV_THUMB2, idiv_thumb2); 
                HELPER(iWMMXt     , iwmmxt     ); 
                HELPER(AES        , aes        ); 
                HELPER(PMULL      , pmull      ); 
                HELPER(SHA1       , sha1       ); 
                HELPER(SHA2       , sha2       ); 
                HELPER(CRC32      , crc32      );
                #undef HELPER

            #elif defined(FE_HW_TARGET_ARM64)

                fe_dbg_assert(family == ANDROID_CPU_FAMILY_ARM64);
                arm64->cpuid = android_getCpuIdArm();
                #define HELPER(CST,cst) \
    if(feats & ANDROID_CPU_ARM64_FEATURE_##CST) arm64->has_##cst = true
                HELPER(FP   , fp   );
                HELPER(ASIMD, asimd);
                HELPER(AES  , aes  );
                HELPER(PMULL, pmull);
                HELPER(SHA1 , sha1 );
                HELPER(SHA2 , sha2 );
                HELPER(CRC32, crc32);
                #undef HELPER

            #endif /* FE_HW_TARGET_ARM32 */

        #elif defined(FE_TARGET_IOS)

            /* On iOS, we don't have to check for cpu features support because
             * our app gets compiled twice into a fat binary - one part with
             * NEON support and the other part without.
             * The OS decides which part to use at lauch-time. 
             * See http://wanderingcoder.net/2010/07/19/ought-arm/
             */
            #ifdef FE_HW_TARGET_ARM_IWMMXT
                arm32->has_wmmx = true;
            #endif
            #ifdef FE_HW_TARGET_ARM_NEON
                arm32->has_neon = true;
            #endif

        #endif /* FE_TARGET_ANDROID */

    }

#endif

void fe_hw_setup(void) {
    cacheinfo_fill(&fe_hw_cacheinfo);
#ifdef FE_HW_TARGET_ARM
    static_arm_features_fill();
#elif defined(FE_HW_TARGET_X86)
    static_x86_cpuid_fill();
#endif
}

