
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


