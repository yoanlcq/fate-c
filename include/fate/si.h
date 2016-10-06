#ifndef FE_SI_H
#define FE_SI_H

#include <fate/stdc.h>
#include <fate/stdc_ext.h>

typedef enum {
    FE_SI_LQ_PRIMITIVE = 0,
    FE_SI_LQ_STRUCT,
    FE_SI_LQ_UNION
} fe_si_lq;

typedef struct fe_si_member fe_si_member;

typedef struct {
    const fe_si_member *members;
    const size_t count;
} fe_si_member_vec;

typedef struct {
    const char *const short_description;
    const char *const long_description;
} fe_si_type_annotations;

typedef struct {
    const fe_si_lq lq;
    const fe_si_member_vec mvec;
    const char *const name;
    const size_t size, psize;
    const fe_si_type_annotations annotations;
} fe_si_type;

typedef size_t (*fe_si_getcount_callback)(const void *const arg);

typedef enum {
    FE_SI_MB_VAL=0, //Single value
    FE_SI_MB_PTR,   //Pointer to unknown count
    FE_SI_MB_VEC,   //Pointer to computable count using callback and arg
    FE_SI_MB_ARY    //Pointer to array with count known at compile time
} fe_si_countdesc_type;


typedef struct {
    const fe_si_getcount_callback callback;
    const void *const arg;
} fe_si_countgetter;

typedef struct {
    fe_si_countdesc_type type;
    union {
        fe_si_countgetter countgetter;
        size_t count;
    };
} fe_si_countdesc;

typedef struct {
    const char *const short_description;
    const char *const long_description;
} fe_si_member_annotations;

struct fe_si_member {
    const fe_si_type *const type;
    const char *const name;
    const size_t offset;
    const fe_si_countdesc countdesc;
    const fe_si_member_annotations annotations;
};

typedef void (*fe_si_inspect_callback)(const fe_si_member *const *mstack, size_t mstack_len, const void *pval, void *userdata);

#define fe_si_inspect(t, pobj, callback, userdata) \
        fe_si_inspect_real(&fe_si_type_##t, NULL, 0, pobj, "", callback, userdata)

void fe_si_inspect_real(const fe_si_type *const t, 
                     fe_si_member const **mstack,
                     size_t mstack_len,
                     const void *const pobj, 
                     const char *const objname,
                     fe_si_inspect_callback callback,
                     void *userdata);

size_t fe_si_member_get_elem_count(const fe_si_member *m);


#define FE_SI_PRIV_PRIM_TYPE(t) \
extern const fe_si_type fe_si_type_##t;
#include <fate/si/priv_primtypes.inl>
#undef FE_SI_PRIV_PRIM_TYPE

#endif /* FE_SI_H */
