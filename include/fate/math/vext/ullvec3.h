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

#ifndef FE_MATH_VEXT_ULLVEC3_H
#define FE_MATH_VEXT_ULLVEC3_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_ULLVEC3_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_ULLVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define fe_ullvec3_shuffle(v,a,b,c,d) \
                __builtin_shufflevector(v,v,a,b,c,d)
    #define fe_ullvec3_shuffle2(u,v,a,b,c,d) \
                __builtin_shufflevector(u,v,a,b,c,d)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_ULLVEC3_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(uint64_t))))
    #define FE_ULLVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define fe_ullvec3_shuffle(v,a,b,c,d)    __builtin_shuffle(v,(fe_ullvec4){a,b,c,d})
    #define fe_ullvec3_shuffle2(u,v,a,b,c,d) __builtin_shuffle(u,v,(fe_ullvec4){a,b,c,d})
#endif
#endif

#ifndef FE_ULLVEC3_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>

#include <fate/math/vext/ullvec4.h> /* Needed for __builtin_shuffle() */
#include <fate/math/vext/ullvec4.h>
typedef fe_ullvec4 fe_ullvec3;


struct FE_ULLVEC3_PACKED_ATTR fe_ullvec3_color {
    uint64_t r;
    uint64_t g;
    uint64_t b;
    /* No alpha component. */
};
typedef struct fe_ullvec3_color fe_ullvec3_color;

struct FE_ULLVEC3_PACKED_ATTR fe_ullvec3_coord {
    uint64_t x;
    uint64_t y;
    uint64_t z;
    /* No w component. */
};
typedef struct fe_ullvec3_coord fe_ullvec3_coord;

#define fe_ullvec3_as_array(v) (&(v)[0])
#define fe_ullvec3_as_color(v) ((fe_ullvec3_color*)fe_ullvec3_as_array(v))
#define fe_ullvec3_as_coord(v) ((fe_ullvec3_coord*)fe_ullvec3_as_array(v))

#define fe_ullvec3_add(s,a,b)   ((*(s))=(*(a))+(*(b)))
#define fe_ullvec3_sub(s,a,b)   ((*(s))=(*(a))-(*(b)))
#define fe_ullvec3_scale(r,v,s) ((*(r))=(*(v))*(*(s)))
#define fe_ullvec3_dot(a,b) fe_ullvec3_mul_inner(a,b)
static inline uint64_t fe_ullvec3_mul_inner(const fe_ullvec3 *a, const fe_ullvec3 *b) {
    fe_ullvec3 v = (*a)*(*b);
    return v[0]+v[1]+v[2];
}
#define fe_ullvec3_len(v)  sqrt(fe_ullvec3_mul_inner(v, v))
#define fe_ullvec3_lenf(v) sqrtf(fe_ullvec3_mul_inner(v, v))
#define fe_ullvec3_norm(r,v) fe_ullvec3_scale(r, v, 1./fe_ullvec3_len(v))

/* TODO benchmark me */
#define fe_ullvec3_mul_cross(r,a,b) fe_ullvec3p_mul_cross(r,a,b)
#define fe_ullvec3_cross(r,a,b)     fe_ullvec3_mul_cross(r,a,b)
static inline void fe_ullvec3p_mul_cross(fe_ullvec3 *r, const fe_ullvec3 *a, const fe_ullvec3 *b) {
    const fe_ullvec3 la = fe_ullvec3_shuffle(*a, 1, 2, 0, 0);
    const fe_ullvec3 rb = fe_ullvec3_shuffle(*b, 1, 2, 0, 0);
    const fe_ullvec3 lb = fe_ullvec3_shuffle(*b, 2, 0, 1, 0);
    const fe_ullvec3 ra = fe_ullvec3_shuffle(*a, 2, 0, 1, 0);
    *r = la*lb - ra*rb;
    
}

/* TODO should be discarded if proven to be less efficient. */
#define fe_ullvec3_mul_cross_naive(r,a,b) fe_ullvec3p_mul_cross_naive(&(r),a,b)
static inline void fe_ullvec3p_mul_cross_naive(fe_ullvec3 *r, const fe_ullvec3 *a, const fe_ullvec3 *b) {
    (*r)[0] = (*a)[1]*(*b)[2] - (*a)[2]*(*b)[1];
    (*r)[1] = (*a)[2]*(*b)[0] - (*a)[0]*(*b)[2];
    (*r)[2] = (*a)[0]*(*b)[1] - (*a)[1]*(*b)[0];
    
}

static inline void fe_ullvec3_reflect(fe_ullvec3 *r, const fe_ullvec3 *v, const fe_ullvec3 *n) {
    const uint64_t p = 2*fe_ullvec3_mul_inner(v, n);
    *r = (*v)-p*(*n);
}

#endif /* FE_MATH_VEXT_ULLVEC3_H */
