/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

/* This file was generated automagically from genvex.h.m4, using Make and 
 * GNU M4. Therefore, it's pointless to edit it. Edit genvex.h.m4 instead. 
 *
 * Thanks to Datenwolf for providing his 'linmath.h' library under the WTFPL
 * license, which was used as a reference to implement this file's 
 * functionalities.
 */

#ifndef FE_MATH_VEXT_I64V3_H
#define FE_MATH_VEXT_I64V3_H

/* Feature test section */

#include <stddef.h>
#include <fate/defs.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#include <fate/math/vext/u64v4.h> /* Needed for __builtin_shuffle() */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_I64V3_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_I64V3_PACKED_ATTR  __attribute__((__packed__))
    #define fe_i64v3_shuffle(r,v,...) \
                do (r)->vx = __builtin_shufflevector((v)->vx,(v)->vx,__VA_ARGS__); while(0)
    #define fe_i64v3_shuffle2(r,u,v,...) \
                do (r)->vx = __builtin_shufflevector((u)->vx,(v)->vx,__VA_ARGS__); while(0)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_I64V3_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(int64_t))))
    #define FE_I64V3_PACKED_ATTR  __attribute__((__packed__))
    #define fe_i64v3_shuffle(r,v,...)    \
        do (r)->vx = __builtin_shuffle((v)->vx, ((fe_u64v4){.vx={__VA_ARGS__}}).vx); while(0)
    #define fe_i64v3_shuffle2(r,u,v,...) \
        do (r)->vx = __builtin_shuffle((u)->vx, (v)->vx, ((fe_u64v4){.vx={__VA_ARGS__}}).vx); while(0)
#endif
#endif

#include <fate/math/vext/i64v4.h>
typedef fe_i64v4vext fe_i64v3vext;


#ifndef FE_I64V3_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

typedef struct { 
    union {
        fe_i64v3vext vx;
        int64_t at[3];
        struct {
            int64_t r;
            int64_t g;
            int64_t b;
            /* No alpha component. */
        };
        struct {
            int64_t x;
            int64_t y;
            int64_t z;
            /* No w component. */
        };
        struct {
            int64_t s;
            int64_t t;
            int64_t p;
            /* No q component. */
        };
    };
} fe_i64v3;

fe_static_assert(offsetof(fe_i64v3, r) == offsetof(fe_i64v3, at[0]), "");
fe_static_assert(offsetof(fe_i64v3, g) == offsetof(fe_i64v3, at[1]), "");
fe_static_assert(offsetof(fe_i64v3, b) == offsetof(fe_i64v3, at[2]), "");



#define fe_i64v3_add(s,a,b)   ((s).vx = (a).vx + (b).vx)
#define fe_i64v3_sub(s,a,b)   ((s).vx = (a).vx - (b).vx)
#define fe_i64v3_scale(r,v,s) ((r).vx = (v).vx * (s))
#define fe_i64v3_dot(a,b) fe_i64v3_mul_inner(a,b)
static inline int64_t fe_i64v3_mul_inner(const fe_i64v3 *a, const fe_i64v3 *b) {
    fe_i64v3 v = (fe_i64v3) {.vx = a->vx * b->vx};
    return v.vx[0]+v.vx[1]+v.vx[2];
}
#define fe_i64v3_len(v)  sqrt(fe_i64v3_mul_inner(v, v))
#define fe_i64v3_lenf(v) sqrtf(fe_i64v3_mul_inner(v, v))
#define fe_i64v3_norm(r,v) fe_i64v3_scale(r, v, 1./fe_i64v3_len(v))

/* TODO benchmark me */
#define fe_i64v3_mul_cross(r,a,b) fe_i64v3p_mul_cross(r,a,b)
#define fe_i64v3_cross(r,a,b)     fe_i64v3_mul_cross(r,a,b)
static inline void fe_i64v3p_mul_cross(fe_i64v3 *r, const fe_i64v3 *a, const fe_i64v3 *b) {
    fe_i64v3 la; fe_i64v3_shuffle(&la, a, 1, 2, 0, 0);
    fe_i64v3 rb; fe_i64v3_shuffle(&rb, b, 1, 2, 0, 0);
    fe_i64v3 lb; fe_i64v3_shuffle(&lb, b, 2, 0, 1, 0);
    fe_i64v3 ra; fe_i64v3_shuffle(&ra, a, 2, 0, 1, 0);
    r->vx = la.vx*lb.vx - ra.vx*rb.vx;
    
}

/* TODO should be discarded if proven to be less efficient. */
#define fe_i64v3_mul_cross_naive(r,a,b) fe_i64v3p_mul_cross_naive(&(r),a,b)
static inline void fe_i64v3p_mul_cross_naive(fe_i64v3 *r, const fe_i64v3 *a, const fe_i64v3 *b) {
    r->vx[0] = a->vx[1]*b->vx[2] - a->vx[2]*b->vx[1];
    r->vx[1] = a->vx[2]*b->vx[0] - a->vx[0]*b->vx[2];
    r->vx[2] = a->vx[0]*b->vx[1] - a->vx[1]*b->vx[0];
    
}

static inline void fe_i64v3_reflect(fe_i64v3 *r, const fe_i64v3 *v, const fe_i64v3 *n) {
    /* GCC claims to be able to multiply by a scalar, but still throws errors
     * like these with the latest MinGW - w64 :
     *   error: conversion of scalar 'long double' to vector 'fe_f64v4'
     *   {aka const __vector(4) double}' involves truncation
     */
    const int64_t p = 2*fe_i64v3_mul_inner(v, n);
    fe_i64v3 pv;
    pv.vx[0]=pv.vx[1]=pv.vx[2]=p;
    r->vx = v->vx - pv.vx * n->vx;
}

#endif /* FE_MATH_VEXT_I64V3_H */
