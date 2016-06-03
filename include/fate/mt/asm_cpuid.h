
#include <cpuid.h>
#define FE_ASM_HLE_BIT bit_HLE //(1<<4)
#define FE_ASM_RTM_BIT bit_RTM //(1<<11)
inline unsigned fe_asm_cpuid(void) {
    unsigned a, b, c, d;
    if(__get_cpuid_max(0, NULL) < 7) 
        return false;
     __cpuid_count(7, 0, a, b, c, d);
    return b;
}

/* See http://nadeausoftware.com/articles/2012/02/c_c_tip_how_detect_processor_type_using_compiler_predefined_macros#x86andx8664 */
#if defined(i386) \
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
#define FE_X86 1
#endif


