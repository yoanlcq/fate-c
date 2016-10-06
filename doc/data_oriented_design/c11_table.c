/* alias cbz cj_cyberzards */
#include <fate/defs.h>
#include <fate/stdc.h>
#include <fate/stdc_ext.h>

typedef uint16_t cbz_idx;
typedef uint16_t cbz_life;

#ifndef CBZ_LAYOUT
#error "Define CBZ_LAYOUT to 1 or 2."
#endif

#if CBZ_LAYOUT==1
fe_pragma_message("cbz: using layout 1.");

typedef struct { cbz_life v, pad; } cbz_life_col;
typedef struct { cbz_life pad, v; } cbz_regen_col;
typedef struct {
    union {
        cbz_life_col  *life;
        cbz_regen_col *regen;
    };
    size_t count;
} cbz_layout1;
cbz_layout1 cbz;

void cbz_alloc_mass(size_t cnt) {
    cbz.life = malloc(cnt*sizeof *cbz.life);
    assert(cbz.life);
    cbz.count = cnt;
}
void cbz_dealloc_mass(void) {
    free(cbz.life);
}

#elif CBZ_LAYOUT==2
fe_pragma_message("cbz: using layout 2.");

typedef struct { cbz_life v; }  cbz_life_col;
typedef struct { cbz_life v; } cbz_regen_col;
typedef struct {
    cbz_life_col  *life;
    cbz_regen_col *regen;
    size_t count;
} cbz_layout2;
cbz_layout2 cbz;

void cbz_alloc_mass(size_t cnt) {
    cbz.life  = malloc(cnt*sizeof *cbz.life);
    assert(cbz.life);
    cbz.regen = malloc(cnt*sizeof *cbz.regen);
    assert(cbz.regen);
    cbz.count = cnt;
}
void cbz_dealloc_mass(void) {
    free(cbz.life);
    free(cbz.regen);
}

#else
#error "CBZ_LAYOUT has an invalid value."
#endif

/* In-place transform. */
static void cbz_regen_lives(void) {
    size_t i; 
    for(i=0 ; i<cbz.count ; ++i)
        cbz.life[i].v += cbz.regen[i].v;
}

int main(void) {
    static const size_t cnt = 1<<29;
    printf("Benchmarking for %u cyberzards.\n", (unsigned)cnt);
    cbz_alloc_mass(cnt);
    //TODO: Sequentiality definition :
    // Strong
    // Stream
    cbz_regen_lives();
    cbz_dealloc_mass();
    return EXIT_SUCCESS;
}



