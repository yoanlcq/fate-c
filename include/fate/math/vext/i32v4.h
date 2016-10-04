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

#ifndef FE_MATH_VEXT_I32V4_H
#define FE_MATH_VEXT_I32V4_H

/* Feature test section */

#include <stddef.h>
#include <fate/defs.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>

#include <fate/math/vext/u32v4.h> /* Needed for __builtin_shuffle() */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_I32V4_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_I32V4_PACKED_ATTR  __attribute__((__packed__))
    #define fe_i32v4_shuffle(r,v,...) \
                do (r)->vx = __builtin_shufflevector((v)->vx,(v)->vx,__VA_ARGS__); while(0)
    #define fe_i32v4_shuffle2(r,u,v,...) \
                do (r)->vx = __builtin_shufflevector((u)->vx,(v)->vx,__VA_ARGS__); while(0)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_I32V4_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(int32_t))))
    #define FE_I32V4_PACKED_ATTR  __attribute__((__packed__))
    #define fe_i32v4_shuffle(r,v,...)    \
        do (r)->vx = __builtin_shuffle((v)->vx, ((fe_u32v4){.vx={__VA_ARGS__}}).vx); while(0)
    #define fe_i32v4_shuffle2(r,u,v,...) \
        do (r)->vx = __builtin_shuffle((u)->vx, (v)->vx, ((fe_u32v4){.vx={__VA_ARGS__}}).vx); while(0)
#endif
#endif

typedef int32_t fe_i32v4vext FE_I32V4_SIZE_ATTR(4);


#ifndef FE_I32V4_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

typedef struct { 
    union {
        fe_i32v4vext vx;
        int32_t at[4];
        struct {
            int32_t r;
            int32_t g;
            int32_t b;
            int32_t a;
        };
        struct {
            int32_t x;
            int32_t y;
            int32_t z;
            int32_t w;
        };
        struct {
            int32_t s;
            int32_t t;
            int32_t p;
            int32_t q;
        };
    };
} fe_i32v4;

fe_static_assert(offsetof(fe_i32v4, r) == offsetof(fe_i32v4, at[0]), "");
fe_static_assert(offsetof(fe_i32v4, g) == offsetof(fe_i32v4, at[1]), "");
fe_static_assert(offsetof(fe_i32v4, b) == offsetof(fe_i32v4, at[2]), "");
fe_static_assert(offsetof(fe_i32v4, a) == offsetof(fe_i32v4, at[3]), "");


#define fe_i32v4_add(s,a,b)   ((s).vx = (a).vx + (b).vx)
#define fe_i32v4_sub(s,a,b)   ((s).vx = (a).vx - (b).vx)
#define fe_i32v4_scale(r,v,s) ((r).vx = (v).vx * (s))
#define fe_i32v4_dot(a,b) fe_i32v4_mul_inner(a,b)
static inline int32_t fe_i32v4_mul_inner(const fe_i32v4 *a, const fe_i32v4 *b) {
    fe_i32v4 v = (fe_i32v4) {.vx = a->vx * b->vx};
    return v.vx[0]+v.vx[1]+v.vx[2]+v.vx[3];
}
#define fe_i32v4_len(v)  sqrt(fe_i32v4_mul_inner(v, v))
#define fe_i32v4_lenf(v) sqrtf(fe_i32v4_mul_inner(v, v))
#define fe_i32v4_norm(r,v) fe_i32v4_scale(r, v, 1./fe_i32v4_len(v))

/* TODO benchmark me */
#define fe_i32v4_mul_cross(r,a,b) fe_i32v4p_mul_cross(r,a,b)
#define fe_i32v4_cross(r,a,b)     fe_i32v4_mul_cross(r,a,b)
static inline void fe_i32v4p_mul_cross(fe_i32v4 *r, const fe_i32v4 *a, const fe_i32v4 *b) {
    fe_i32v4 la; fe_i32v4_shuffle(&la, a, 1, 2, 0, 0);
    fe_i32v4 rb; fe_i32v4_shuffle(&rb, b, 1, 2, 0, 0);
    fe_i32v4 lb; fe_i32v4_shuffle(&lb, b, 2, 0, 1, 0);
    fe_i32v4 ra; fe_i32v4_shuffle(&ra, a, 2, 0, 1, 0);
    r->vx = la.vx*lb.vx - ra.vx*rb.vx;
    r->vx[3] = 1;
}

/* TODO should be discarded if proven to be less efficient. */
#define fe_i32v4_mul_cross_naive(r,a,b) fe_i32v4p_mul_cross_naive(&(r),a,b)
static inline void fe_i32v4p_mul_cross_naive(fe_i32v4 *r, const fe_i32v4 *a, const fe_i32v4 *b) {
    r->vx[0] = a->vx[1]*b->vx[2] - a->vx[2]*b->vx[1];
    r->vx[1] = a->vx[2]*b->vx[0] - a->vx[0]*b->vx[2];
    r->vx[2] = a->vx[0]*b->vx[1] - a->vx[1]*b->vx[0];
    r->vx[3] = 1;
}

static inline void fe_i32v4_reflect(fe_i32v4 *r, const fe_i32v4 *v, const fe_i32v4 *n) {
    /* GCC claims to be able to multiply by a scalar, but still throws errors
     * like these with the latest MinGW - w64 :
     *   error: conversion of scalar 'long double' to vector 'fe_f64v4'
     *   {aka const __vector(4) double}' involves truncation
     */
    const int32_t p = 2*fe_i32v4_mul_inner(v, n);
    fe_i32v4 pv;
    pv.vx[0]=pv.vx[1]=pv.vx[2]=pv.vx[3]=p;
    r->vx = v->vx - pv.vx * n->vx;
}

#endif /* FE_MATH_VEXT_I32V4_H */
