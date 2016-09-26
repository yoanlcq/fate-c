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

#ifndef FE_MATH_VEXT_U64V2_H
#define FE_MATH_VEXT_U64V2_H

/* Feature test section */

#include <stddef.h>
#include <fate/defs.h>

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_U64V2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_U64V2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_u64v2_shuffle(v,a,b,c,d) \
                __builtin_shufflevector((v)->vx,(v)->vx,a,b,c,d)
    #define fe_u64v2_shuffle2(u,v,a,b,c,d) \
                __builtin_shufflevector((u)->vx,(v)->vx,a,b,c,d)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_U64V2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(uint64_t))))
    #define FE_U64V2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_u64v2_shuffle(v,a,b,c,d)    __builtin_shuffle((v)->vx,(fe_mask){a,b,c,d})
    #define fe_u64v2_shuffle2(u,v,a,b,c,d) __builtin_shuffle((u)->vx,(v)->vx,(fe_mask){a,b,c,d})
#endif
#endif

#ifndef FE_U64V2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>


typedef uint64_t fe_u64v2vext FE_U64V2_SIZE_ATTR(2);


typedef struct { 
    union {
        fe_u64v2vext vx;
        uint64_t at[2];
        struct {
            uint64_t r;
            uint64_t g;
            /* No blue component. */
            /* No alpha component. */
        };
        struct {
            uint64_t x;
            uint64_t y;
            /* No z component. */
            /* No w component. */
        };
        struct {
            uint64_t s;
            uint64_t t;
            /* No p component. */
            /* No q component. */
        };
    };
} fe_u64v2;

FE_COMPILETIME_ASSERT(offsetof(fe_u64v2, r) == offsetof(fe_u64v2, at[0]), "");
FE_COMPILETIME_ASSERT(offsetof(fe_u64v2, g) == offsetof(fe_u64v2, at[1]), "");




#define fe_u64v2_add(s,a,b)   ((s).vx = (a).vx + (b).vx)
#define fe_u64v2_sub(s,a,b)   ((s).vx = (a).vx - (b).vx)
#define fe_u64v2_scale(r,v,s) ((r).vx = (v).vx * (s))
#define fe_u64v2_dot(a,b) fe_u64v2_mul_inner(a,b)
static inline uint64_t fe_u64v2_mul_inner(const fe_u64v2 *a, const fe_u64v2 *b) {
    fe_u64v2 v = (fe_u64v2) {.vx = a->vx * b->vx};
    return v.vx[0]+v.vx[1];
}
#define fe_u64v2_len(v)  sqrt(fe_u64v2_mul_inner(v, v))
#define fe_u64v2_lenf(v) sqrtf(fe_u64v2_mul_inner(v, v))
#define fe_u64v2_norm(r,v) fe_u64v2_scale(r, v, 1./fe_u64v2_len(v))

/* No cross product for fe_u64v2. */

static inline void fe_u64v2_reflect(fe_u64v2 *r, const fe_u64v2 *v, const fe_u64v2 *n) {
    /* GCC claims to be able to multiply by a scalar, but still throws errors
     * like these with the latest MinGW - w64 :
     *   error: conversion of scalar 'long double' to vector 'fe_dvec4 
     *   {aka const __vector(4) double}' involves truncation
     */
    const uint64_t p = 2*fe_u64v2_mul_inner(v, n);
    fe_u64v2 pv;
    pv.vx[0]=pv.vx[1]=p;
    r->vx = v->vx - pv.vx * n->vx;
}

#endif /* FE_MATH_VEXT_U64V2_H */
