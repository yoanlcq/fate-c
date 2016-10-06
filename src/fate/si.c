#include <fate/si.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define FE_SI_PRIV_PRIM_TYPE(t) \
const fe_si_type fe_si_type_##t = { FE_SI_LQ_PRIMITIVE, {0}, #t, sizeof(t), sizeof(t*), {0} };
#include <fate/si/priv_primtypes.inl>
#undef FE_SI_PRIV_PRIM_TYPE

static void fe_si_inspect_real_recursive(fe_si_type *t, 
                     fe_si_member *** mstack,
                     size_t mstack_len,
                     const void *const pobj, 
                     const char *const objname,
                     fe_si_inspect_callback callback,
                     void *userdata) 
{
    size_t i;
    *mstack = realloc(*mstack, (mstack_len+1)*sizeof(fe_si_member*));
    ++mstack_len;
    for(i=0 ; i<t->mvec.count ; ++i) {
        fe_si_member *m = t->mvec.members + i;
        (*mstack)[mstack_len-1] = m;
        const char *pmem = pobj;
        pmem += m->offset;
        callback(*mstack, mstack_len, pmem, userdata);
        if(pmem != pobj)
            fe_si_inspect_real_recursive(m->type, mstack, mstack_len, pmem, m->name, callback, userdata);
    }
}

void fe_si_inspect_real(fe_si_type *t, 
                     fe_si_member **mstack,
                     size_t mstack_len,
                     const void *const pobj, 
                     const char *const objname,
                     fe_si_inspect_callback callback,
                     void *userdata) {
    fe_si_inspect_real_recursive(t, &mstack, mstack_len, pobj, objname, callback, userdata);
    free(mstack);
}


size_t fe_si_member_get_elem_count(const fe_si_member *m) {
    const fe_si_countdesc *cd = &m->countdesc;
    switch(cd->type) {
    case FE_SI_MB_VAL: return 1; break;
    case FE_SI_MB_PTR: return 1; break;
    case FE_SI_MB_VEC: return cd->countgetter.callback(cd->countgetter.arg); break;
    case FE_SI_MB_ARY: return cd->count; break;
    }
    return 0;
}

