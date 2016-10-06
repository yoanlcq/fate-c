#include <gm_types.h>

static size_t static_get_item_count(const void *const arg) {
    return 5;
}

#define FE_SI_GENERATE_FROM "gm_types.inl"
#include <fate/si/generate_for_source.inl>
#undef FE_SI_GENERATE_FROM

