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

#ifndef FE_MATH_VEXT_WV2_H
#define FE_MATH_VEXT_WV2_H

/* Feature test section */

#include <stddef.h>
#include <fate/defs.h>

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_WV2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_WV2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_wv2_shuffle(r,v,...) \
                do (r)->vx = __builtin_shufflevector((v)->vx,(v)->vx,__VA_ARGS__); while(0)
    #define fe_wv2_shuffle2(r,u,v,...) \
                do (r)->vx = __builtin_shufflevector((u)->vx,(v)->vx,__VA_ARGS__); while(0)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_WV2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(fe_space_unit))))
    #define FE_WV2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_wv2_shuffle(r,v,...)    \
        do (r)->vx = __builtin_shuffle((v)->vx, ((fe_mask){.vx={__VA_ARGS__}}).vx); while(0)
    #define fe_wv2_shuffle2(r,u,v,...) \
        do (r)->vx = __builtin_shuffle((u)->vx, (v)->vx, ((fe_mask){.vx={__VA_ARGS__}}).vx); while(0)
#endif
#endif

#ifndef FE_WV2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
#include <fate/units.h>

typedef fe_space_unit fe_wv2vext FE_WV2_SIZE_ATTR(2);


typedef struct { 
    union {
        fe_wv2vext vx;
        fe_space_unit at[2];
        struct {
            fe_space_unit r;
            fe_space_unit g;
            /* No blue component. */
            /* No alpha component. */
        };
        struct {
            fe_space_unit x;
            fe_space_unit y;
            /* No z component. */
            /* No w component. */
        };
        struct {
            fe_space_unit s;
            fe_space_unit t;
            /* No p component. */
            /* No q component. */
        };
    };
} fe_wv2;

static_assert(offsetof(fe_wv2, r) == offsetof(fe_wv2, at[0]), "");
static_assert(offsetof(fe_wv2, g) == offsetof(fe_wv2, at[1]), "");




#define fe_wv2_add(s,a,b)   ((s).vx = (a).vx + (b).vx)
#define fe_wv2_sub(s,a,b)   ((s).vx = (a).vx - (b).vx)
#define fe_wv2_scale(r,v,s) ((r).vx = (v).vx * (s))
#define fe_wv2_dot(a,b) fe_wv2_mul_inner(a,b)
static inline fe_space_unit fe_wv2_mul_inner(const fe_wv2 *a, const fe_wv2 *b) {
    fe_wv2 v = (fe_wv2) {.vx = a->vx * b->vx};
    return v.vx[0]+v.vx[1];
}
#define fe_wv2_len(v)  sqrt(fe_wv2_mul_inner(v, v))
#define fe_wv2_lenf(v) sqrtf(fe_wv2_mul_inner(v, v))
#define fe_wv2_norm(r,v) fe_wv2_scale(r, v, 1./fe_wv2_len(v))

/* No cross product for fe_wv2. */

static inline void fe_wv2_reflect(fe_wv2 *r, const fe_wv2 *v, const fe_wv2 *n) {
    /* GCC claims to be able to multiply by a scalar, but still throws errors
     * like these with the latest MinGW - w64 :
     *   error: conversion of scalar 'long double' to vector 'fe_f64v4'
     *   {aka const __vector(4) double}' involves truncation
     */
    const fe_space_unit p = 2*fe_wv2_mul_inner(v, n);
    fe_wv2 pv;
    pv.vx[0]=pv.vx[1]=p;
    r->vx = v->vx - pv.vx * n->vx;
}

#endif /* FE_MATH_VEXT_WV2_H */
