#include <stdio.h>
#include <fate/si.h>
#include <gm_types.h>

void inspect(const fe_si_member *const *mstack, size_t mstack_len,
             const void *pval, void *name) {

    const fe_si_member *m = mstack[mstack_len-1];
    if(!(m->type->lq == FE_SI_LQ_PRIMITIVE || m->countdesc.type==FE_SI_MB_PTR)) 
        return;
    
    fputs(name, stdout);
    size_t mi;
    for(mi=0 ; mi<mstack_len ; ++mi) {
        m = mstack[mi];
        printf(".%s", m->name);
    }

    size_t cnt = fe_si_member_get_elem_count(m);
    printf(" = (%s%s)",
        m->type->name, m->countdesc.type==FE_SI_MB_VAL ? "" : "*"
    );
    size_t i;
    if(m->type == &fe_si_type_uint16_t)
        for(i=0 ; i<cnt ; ++i) printf(" %hu", ((uint16_t*)pval)[i]);
    else if(m->type == &fe_si_type_uint8_t)
        for(i=0 ; i<cnt ; ++i) printf(" %hhu", ((uint8_t*)pval)[i]);
    else
        printf(" %p", *(void**)pval);
    putchar('\n');
}

int main(void) {
    color_rgb rgb = {8,4,2};
    fe_si_inspect(color_rgb, &rgb, inspect, "my_color");
    gr_grisui kutee = {NULL, {0,1,2,3}, NULL, rgb};
    fe_si_inspect(gr_grisui, &kutee, inspect, "Kutee");
    return 0;
}
