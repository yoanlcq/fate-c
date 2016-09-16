#include <fate/hash.h>
#include <fate/hw.h>

uint64_t fe_hash_sdbm(const char *str, size_t len) {
    uint64_t hash; 
    size_t i;
    for(hash=i=0 ; i<len && str[i] ; ++i)
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    return hash;
}





/* From http://create.stephan-brumme.com/crc32/ */

/* Use the below code to generate the LUT for any polynomial.
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
int main(void) {
    uint32_t polynomial = 0x1EDC6F41;
    uint32_t lut[16];
    size_t i, j;
    for(i=0; i < 16; i++) {
        uint32_t crc = i * 16;
        for(j=0; j < 8; j++)
            crc = (crc >> 1) ^ (-(int32_t)(crc & 1) & polynomial);
        lut[i] = crc;
    }
    for(i=0 ; i<15 ; ++i)
        printf("0x%.8"PRIX32",\n", lut[i]);
    printf("0x%.8"PRIX32"\n", lut[i]);
    return 0;
}
*/

static uint32_t crc32c_halfbyte(const void* data, size_t len, uint32_t crc32c) {
    /* Beware ! This LUT depends on the polynomial. See above. */
    static const uint32_t lut[16] = {
        0x00000000, 0x15DECED9, 0x16054331, 0x03DB8DE8, 0x11B258E1, 0x046C9638, 
        0x07B71BD0, 0x1269D509, 0x1EDC6F41, 0x0B02A198, 0x08D92C70, 0x1D07E2A9, 
        0x0F6E37A0, 0x1AB0F979, 0x196B7491, 0x0CB5BA48
    };
    const uint8_t* current = data;
    crc32c = ~crc32c;
    while(--len) {
        crc32c = lut[(crc32c ^  *current      ) & 0xF] ^ (crc32c >> 4);
        crc32c = lut[(crc32c ^ (*current >> 4)) & 0xF] ^ (crc32c >> 4);
        ++current;
    }
    return ~crc32c;
}


#if defined(FE_HW_TARGET_ARM64) /* ARMv8 */ && defined(__GNUC__)
#define STATIC_HAS_CRC32_ARMV8
#endif

#ifdef STATIC_HAS_CRC32_ARMV8
/* See https://wiki.linaro.org/LEG/Engineering/OPTIM/CRC#Optional_CRC_Instructions_in_ARMv8 */
#define CRC32CX(crc, value) __asm__("crc32cx %w[c], %w[c], %x[v]":[c]"+r"(crc):[v]"r"(value))
#define CRC32CW(crc, value) __asm__("crc32cw %w[c], %w[c], %w[v]":[c]"+r"(crc):[v]"r"(value))
#define CRC32CH(crc, value) __asm__("crc32ch %w[c], %w[c], %w[v]":[c]"+r"(crc):[v]"r"(value))
#define CRC32CB(crc, value) __asm__("crc32cb %w[c], %w[c], %w[v]":[c]"+r"(crc):[v]"r"(value))
static uint32_t crc32c_armv8(const void *data, size_t len, uint32_t crc32c)
{
    const uint8_t *buffer = data;
    uint64_t quotient = len / sizeof(uint64_t);
    while(quotient--) {
        uint64_t val = *(uint64_t*)buffer;
        CRC32CX(crc32c, val);
        buffer += 8;
    }
    if(len & 4) {
        uint32_t val = *(uint32_t*)buffer;
        CRC32CW(crc32c, val);
        buffer += 4;
    }
    if(len & 2) {
        uint16_t val = *(uint16_t*)buffer;
        CRC32CH(crc32c, val);
        buffer += 2;
    }
    if(len & 1) {
        uint8_t val = *(uint8_t*)buffer;
        CRC32CB(crc32c, val);
    }
    return crc32c;
}
#endif /* STATIC_HAS_CRC32_ARMV8 */


#if defined(FE_HW_TARGET_X86) && defined(__SSE4_2__)
/* See http://blog.jiubao.org/2012/07/sse42-crc32c.html */
static uint32_t crc32c_sse4_2(const void *data, size_t len, uint32_t crc32c)
{
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
}
#endif /* FE_HW_TARGET_X86 */

uint32_t (*fe_hash_crc32c)(const void *, size_t, uint32_t) 
    = crc32c_halfbyte;

void fe_hash_setup(void) {
#if defined(FE_HW_TARGET_X86) && defined(__SSE4_2__)
    if(fe_hw_x86_cpu_info.has_sse4_2)
        fe_hash_crc32c = crc32c_sse4_2;
#elif defined(FE_HW_TARGET_ARM64) && defined(STATIC_HAS_CRC32_ARMV8)
    if(fe_hw_arm64_cpu_info.has_crc32)
        fe_hash_crc32c = crc32c_armv8;
#elif defined(FE_HW_TARGET_ARM32) && defined(STATIC_HAS_CRC32_ARMV8)
    if(fe_hw_arm32_cpu_info.has_crc32)
        fe_hash_crc32c = crc32c_armv8;
#endif
}
