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
changecom()
/* This file was generated automagically from generator, using Make and 
 * GNU M4. Therefore, it's pointless to edit it. Edit generator instead. 
 *
 * Thanks to Datenwolf for providing his 'linmath.h' library under the WTFPL
 * license, which was used as a reference to implement this file's 
 * functionalities.
 */

define(`VEC',translit(vec, `a-z', `A-Z'))dnl
define(`NS',translit(ns, `a-z', `A-Z'))dnl
dnl
#ifndef PREFIX`'VEC`'_H
#define PREFIX`'VEC`'_H

/* Feature test section */

#include <stddef.h>
#include <fate/defs.h>

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define NS`'VEC`'_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define NS`'VEC`'_PACKED_ATTR  __attribute__((__packed__))
    #define ns`'vec`'_shuffle(r,v,...) \
                do (r)->vx = __builtin_shufflevector((v)->vx,(v)->vx,__VA_ARGS__); while(0)
    #define ns`'vec`'_shuffle2(r,u,v,...) \
                do (r)->vx = __builtin_shufflevector((u)->vx,(v)->vx,__VA_ARGS__); while(0)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define NS`'VEC`'_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(type))))
    #define NS`'VEC`'_PACKED_ATTR  __attribute__((__packed__))
    #define ns`'vec`'_shuffle(r,v,...)    \
        do (r)->vx = __builtin_shuffle((v)->vx, ((ns`'mask){.vx={__VA_ARGS__}}).vx); while(0)
    #define ns`'vec`'_shuffle2(r,u,v,...) \
        do (r)->vx = __builtin_shuffle((u)->vx, (v)->vx, ((ns`'mask){.vx={__VA_ARGS__}}).vx); while(0)
#endif
#endif

#ifndef NS`'VEC`'_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
ifelse(type,fe_space_unit,#include <fate/units.h>)
ifelse(eval(dim>=3),1,dnl
#include <dir/mask.h> /* Needed for __builtin_shuffle() */)dnl

ifelse(eval(dim!=3),1,dnl
typedef type ns`'vec`'vext NS`'VEC`'_SIZE_ATTR(dim);
,dnl
#include <dir/vec4.h>
typedef ns`'vec4`'vext ns`'vec`'vext;
)dnl


typedef struct { 
    union {
        ns`'vec`'vext vx;
        type at[dim];
        struct {
            type r;
            type g;
            ifelse(eval(dim>=3),1,type b;,/* No blue component. */)
            ifelse(eval(dim>=4),1,type a;,/* No alpha component. */)
        };
        struct {
            type x;
            type y;
            ifelse(eval(dim>=3),1,type z;,/* No z component. */)
            ifelse(eval(dim>=4),1,type w;,/* No w component. */)
        };
        struct {
            type s;
            type t;
            ifelse(eval(dim>=3),1,type p;,/* No p component. */)
            ifelse(eval(dim>=4),1,type q;,/* No q component. */)
        };
    };
} ns`'vec;

static_assert(offsetof(ns`'vec, r) == offsetof(ns`'vec, at[0]), "");
static_assert(offsetof(ns`'vec, g) == offsetof(ns`'vec, at[1]), "");
ifelse(eval(dim>=3),1,static_assert(offsetof(ns`'vec, b) == offsetof(ns`'vec, at[2]), "");)
ifelse(eval(dim>=4),1,static_assert(offsetof(ns`'vec, a) == offsetof(ns`'vec, at[3]), "");)


#define ns`'vec`'_add(s,a,b)   ((s).vx = (a).vx + (b).vx)
#define ns`'vec`'_sub(s,a,b)   ((s).vx = (a).vx - (b).vx)
#define ns`'vec`'_scale(r,v,s) ((r).vx = (v).vx * (s))
#define ns`'vec`'_dot(a,b) ns`'vec`'_mul_inner(a,b)
static inline type ns`'vec`'_mul_inner(const ns`'vec *a, const ns`'vec *b) {
    ns`'vec v = (ns`'vec) {.vx = a->vx * b->vx};
    ifelse(
        dim, 4, return v.vx[0]+v.vx[1]+v.vx[2]+v.vx[3],
        dim, 3, return v.vx[0]+v.vx[1]+v.vx[2],
        dim, 2, return v.vx[0]+v.vx[1],
        type r = 0;
        size_t i;
        for(i=0 ; i<dim ; ++i)
            r += v.at[i];
        return r
    );
}
#define ns`'vec`'_len(v)  sqrt(ns`'vec`'_mul_inner(v, v))
#define ns`'vec`'_lenf(v) sqrtf(ns`'vec`'_mul_inner(v, v))
#define ns`'vec`'_norm(r,v) ns`'vec`'_scale(r, v, 1./ns`'vec`'_len(v))

ifelse(eval(dim>=3),1,
/* TODO benchmark me */
#define ns`'vec`'_mul_cross(r,a,b) ns`'vec`'p_mul_cross(r,a,b)
#define ns`'vec`'_cross(r,a,b)     ns`'vec`'_mul_cross(r,a,b)
static inline void ns`'vec`'p_mul_cross(ns`'vec *r, const ns`'vec *a, const ns`'vec *b) {
    ns`'vec la; ns`'vec`'_shuffle(&la, a, 1, 2, 0, 0);
    ns`'vec rb; ns`'vec`'_shuffle(&rb, b, 1, 2, 0, 0);
    ns`'vec lb; ns`'vec`'_shuffle(&lb, b, 2, 0, 1, 0);
    ns`'vec ra; ns`'vec`'_shuffle(&ra, a, 2, 0, 1, 0);
    r->vx = la.vx*lb.vx - ra.vx*rb.vx;
    ifelse(eval(dim==4),1,r->vx[3] = 1;)
}

/* TODO should be discarded if proven to be less efficient. */
#define ns`'vec`'_mul_cross_naive(r,a,b) ns`'vec`'p_mul_cross_naive(&(r),a,b)
static inline void ns`'vec`'p_mul_cross_naive(ns`'vec *r, const ns`'vec *a, const ns`'vec *b) {
    r->vx[0] = a->vx[1]*b->vx[2] - a->vx[2]*b->vx[1];
    r->vx[1] = a->vx[2]*b->vx[0] - a->vx[0]*b->vx[2];
    r->vx[2] = a->vx[0]*b->vx[1] - a->vx[1]*b->vx[0];
    ifelse(eval(dim==4),1,r->vx[3] = 1;)
}
,/* No cross product for ns`'vec. */
)dnl

static inline void ns`'vec`'_reflect(ns`'vec *r, const ns`'vec *v, const ns`'vec *n) {
    /* GCC claims to be able to multiply by a scalar, but still throws errors
     * like these with the latest MinGW - w64 :
     *   error: conversion of scalar 'long double' to vector 'fe_f64v4'
     *   {aka const __vector(4) double}' involves truncation
     */
    const type p = 2*ns`'vec`'_mul_inner(v, n);
    ns`'vec pv;
    ifelse(
        dim, 4, pv.vx[0]=pv.vx[1]=pv.vx[2]=pv.vx[3]=p,
        dim, 3, pv.vx[0]=pv.vx[1]=pv.vx[2]=p,
        dim, 2, pv.vx[0]=pv.vx[1]=p,
        size_t i;
        for(i=0 ; i<dim ; ++i)
            pv.at[i] = p;
    );
    r->vx = v->vx - pv.vx * n->vx;
}

#endif /* PREFIX`'VEC`'_H */
