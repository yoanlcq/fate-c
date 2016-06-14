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

#ifndef FE_MATH_VEXT_WVEC3_H
#define FE_MATH_VEXT_WVEC3_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_WVEC3_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_WVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define fe_wvec3_shuffle(v,a,b,c,d) \
                __builtin_shufflevector(v,v,a,b,c,d)
    #define fe_wvec3_shuffle2(u,v,a,b,c,d) \
                __builtin_shufflevector(u,v,a,b,c,d)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_WVEC3_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(fe_space_unit))))
    #define FE_WVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define fe_wvec3_shuffle(v,a,b,c,d)    __builtin_shuffle(v,(fe_wvec4){a,b,c,d})
    #define fe_wvec3_shuffle2(u,v,a,b,c,d) __builtin_shuffle(u,v,(fe_wvec4){a,b,c,d})
#endif
#endif

#ifndef FE_WVEC3_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
#include <fate/units.h>
#include <fate/math/vext/wvec4.h> /* Needed for __builtin_shuffle() */
#include <fate/math/vext/wvec4.h>
typedef fe_wvec4 fe_wvec3;


struct FE_WVEC3_PACKED_ATTR fe_wvec3_color {
    fe_space_unit r;
    fe_space_unit g;
    fe_space_unit b;
    /* No alpha component. */
};
typedef struct fe_wvec3_color fe_wvec3_color;

struct FE_WVEC3_PACKED_ATTR fe_wvec3_coord {
    fe_space_unit x;
    fe_space_unit y;
    fe_space_unit z;
    /* No w component. */
};
typedef struct fe_wvec3_coord fe_wvec3_coord;

#define fe_wvec3_as_array(v) (&(v)[0])
#define fe_wvec3_as_color(v) ((fe_wvec3_color*)fe_wvec3_as_array(v))
#define fe_wvec3_as_coord(v) ((fe_wvec3_coord*)fe_wvec3_as_array(v))

#define fe_wvec3_add(s,a,b)   ((s)=(a)+(b))
#define fe_wvec3_sub(s,a,b)   ((s)=(a)-(b))
#define fe_wvec3_scale(r,v,s) ((r)=(v)*(s))
#define fe_wvec3_dot(a,b) fe_wvec3_mul_inner(a,b)
static inline fe_space_unit fe_wvec3_mul_inner(const fe_wvec3 a, const fe_wvec3 b) {
    fe_wvec3 v = a*b;
    return v[0]+v[1]+v[2];
}
#define fe_wvec3_len(v)  sqrt(fe_wvec3_mul_inner(v, v))
#define fe_wvec3_lenf(v) sqrtf(fe_wvec3_mul_inner(v, v))
#define fe_wvec3_norm(r,v) fe_wvec3_scale(r, v, 1./fe_wvec3_len(v))

/* TODO benchmark me */
#define fe_wvec3_mul_cross(r,a,b) fe_wvec3p_mul_cross(&(r),a,b)
#define fe_wvec3_cross(r,a,b)     fe_wvec3_mul_cross(r,a,b)
static inline void fe_wvec3p_mul_cross(fe_wvec3 *r, const fe_wvec3 a, const fe_wvec3 b) {
    const fe_wvec3 la = fe_wvec3_shuffle(a, 1, 2, 0, 0);
    const fe_wvec3 rb = fe_wvec3_shuffle(b, 1, 2, 0, 0);
    const fe_wvec3 lb = fe_wvec3_shuffle(b, 2, 0, 1, 0);
    const fe_wvec3 ra = fe_wvec3_shuffle(a, 2, 0, 1, 0);
    *r = la*lb - ra*rb;
    
}

/* TODO should be discarded if proven to be less efficient. */
#define fe_wvec3_mul_cross_naive(r,a,b) fe_wvec3p_mul_cross_naive(&(r),a,b)
static inline void fe_wvec3p_mul_cross_naive(fe_wvec3 *r, const fe_wvec3 a, const fe_wvec3 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    
}

#define fe_wvec3_reflect(r,v,n) fe_wvec3p_reflect(&r,v,n)
static inline void fe_wvec3p_reflect(fe_wvec3 *r, const fe_wvec3 v, const fe_wvec3 n) {
    const fe_space_unit p = 2*fe_wvec3_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_WVEC3_H */
