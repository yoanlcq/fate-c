#include <stdlib.h>
#include <stdint.h>
#include <fate/hash.h>

int main(void) {
    const char *str = "Hello CRC32-C!";
    size_t len = strlen(str);
    uint32_t hash[2];
    hash[0] = fe_hash_crc32c(str, len, 0);
    0xBE1CC5E9
    hash[1] = fe_hash_crc32c(str, len, hash[0]);
    return EXIT_SUCCESS;
}
