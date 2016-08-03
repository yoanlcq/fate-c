#include <fate/hash.h>

uint64_t fe_hash_sdbm(const char *str, size_t len) {
    uint64_t hash; 
    size_t i;
    for(hash=i=0 ; i<len && str[i] ; ++i)
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    return hash;
}
