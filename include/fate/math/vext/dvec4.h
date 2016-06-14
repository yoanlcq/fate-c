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

#ifndef FE_MATH_VEXT_DVEC4_H
#define FE_MATH_VEXT_DVEC4_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_DVEC4_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_DVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define fe_dvec4_shuffle(v,a,b,c,d) \
                __builtin_shufflevector(v,v,a,b,c,d)
    #define fe_dvec4_shuffle2(u,v,a,b,c,d) \
                __builtin_shufflevector(u,v,a,b,c,d)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_DVEC4_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(double))))
    #define FE_DVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define fe_dvec4_shuffle(v,a,b,c,d)    __builtin_shuffle(v,(fe_ullvec4){a,b,c,d})
    #define fe_dvec4_shuffle2(u,v,a,b,c,d) __builtin_shuffle(u,v,(fe_ullvec4){a,b,c,d})
#endif
#endif

#ifndef FE_DVEC4_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>

#include <fate/math/vext/ullvec4.h> /* Needed for __builtin_shuffle() */
typedef double fe_dvec4 FE_DVEC4_SIZE_ATTR(4);


struct FE_DVEC4_PACKED_ATTR fe_dvec4_color {
    double r;
    double g;
    double b;
    double a;
};
typedef struct fe_dvec4_color fe_dvec4_color;

struct FE_DVEC4_PACKED_ATTR fe_dvec4_coord {
    double x;
    double y;
    double z;
    double w;
};
typedef struct fe_dvec4_coord fe_dvec4_coord;

#define fe_dvec4_as_array(v) (&(v)[0])
#define fe_dvec4_as_color(v) ((fe_dvec4_color*)fe_dvec4_as_array(v))
#define fe_dvec4_as_coord(v) ((fe_dvec4_coord*)fe_dvec4_as_array(v))

#define fe_dvec4_add(s,a,b)   ((s)=(a)+(b))
#define fe_dvec4_sub(s,a,b)   ((s)=(a)-(b))
#define fe_dvec4_scale(r,v,s) ((r)=(v)*(s))
#define fe_dvec4_dot(a,b) fe_dvec4_mul_inner(a,b)
static inline double fe_dvec4_mul_inner(const fe_dvec4 a, const fe_dvec4 b) {
    fe_dvec4 v = a*b;
    return v[0]+v[1]+v[2]+v[3];
}
#define fe_dvec4_len(v)  sqrt(fe_dvec4_mul_inner(v, v))
#define fe_dvec4_lenf(v) sqrtf(fe_dvec4_mul_inner(v, v))
#define fe_dvec4_norm(r,v) fe_dvec4_scale(r, v, 1./fe_dvec4_len(v))

/* TODO benchmark me */
#define fe_dvec4_mul_cross(r,a,b) fe_dvec4p_mul_cross(&(r),a,b)
#define fe_dvec4_cross(r,a,b)     fe_dvec4_mul_cross(r,a,b)
static inline void fe_dvec4p_mul_cross(fe_dvec4 *r, const fe_dvec4 a, const fe_dvec4 b) {
    const fe_dvec4 la = fe_dvec4_shuffle(a, 1, 2, 0, 0);
    const fe_dvec4 rb = fe_dvec4_shuffle(b, 1, 2, 0, 0);
    const fe_dvec4 lb = fe_dvec4_shuffle(b, 2, 0, 1, 0);
    const fe_dvec4 ra = fe_dvec4_shuffle(a, 2, 0, 1, 0);
    *r = la*lb - ra*rb;
    (*r)[3] = 1;
}

/* TODO should be discarded if proven to be less efficient. */
#define fe_dvec4_mul_cross_naive(r,a,b) fe_dvec4p_mul_cross_naive(&(r),a,b)
static inline void fe_dvec4p_mul_cross_naive(fe_dvec4 *r, const fe_dvec4 a, const fe_dvec4 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    (*r)[3] = 1;
}

#define fe_dvec4_reflect(r,v,n) fe_dvec4p_reflect(&r,v,n)
static inline void fe_dvec4p_reflect(fe_dvec4 *r, const fe_dvec4 v, const fe_dvec4 n) {
    const double p = 2*fe_dvec4_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_DVEC4_H */
