#include <fate/hash.h>
#include <fate/hw.h>

/* See http://blog.jiubao.org/2012/07/sse42-crc32c.html */
uint32_t fe_hash_crc32c_sse4_2(const void *data, size_t len, uint32_t crc32c)
{
#if defined(FE_HW_TARGET_X86) && defined(__SSE4_2__)

#ifdef FE_HW_TARGET_X86_64
#define uintmm uint64_t
#else
#define uintmm uint32_t
#endif
    const uint8_t *buffer = data;
    uintmm quotient = len / sizeof(uintmm);
    while(quotient--) {
#ifdef FE_HW_TARGET_X86_64
        crc32c = _mm_crc32_u64(crc32c, *(uintmm*)buffer);
#else
        crc32c = _mm_crc32_u32(crc32c, *(uintmm*)buffer);
#endif
        buffer += sizeof(uintmm);
    }
#undef uintmm
#ifdef FE_HW_TARGET_X86_64
    if(len & 4) {
        crc32c = _mm_crc32_u32(crc32c, *(uint32_t*)buffer);
        buffer += 4;
    }
#endif
    if(len & 2) {
        crc32c = _mm_crc32_u16(crc32c, *(uint16_t*)buffer);
        buffer += 2;
    }
    if(len & 1)
        crc32c = _mm_crc32_u8(crc32c, *(uint8_t*)buffer);
    return crc32c;
#else
    return 0;
#endif /* FE_HW_TARGET_X86 */
}

