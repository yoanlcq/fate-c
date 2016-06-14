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

#ifndef FE_MATH_VEXT_IVEC4_H
#define FE_MATH_VEXT_IVEC4_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_IVEC4_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_IVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define fe_ivec4_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define fe_ivec4_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_IVEC4_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(int32_t))))
    #define FE_IVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define fe_ivec4_shuffle(v,m)    __builtin_shuffle(v,m)
    #define fe_ivec4_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef FE_IVEC4_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>

#include <fate/math/vext/uvec4.h> /* Needed for __builtin_shuffle() */
typedef int32_t fe_ivec4 FE_IVEC4_SIZE_ATTR(4);


struct FE_IVEC4_PACKED_ATTR fe_ivec4_color {
    int32_t r;
    int32_t g;
    int32_t b;
    int32_t a;
};
typedef struct fe_ivec4_color fe_ivec4_color;

struct FE_IVEC4_PACKED_ATTR fe_ivec4_coord {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t w;
};
typedef struct fe_ivec4_coord fe_ivec4_coord;

#define fe_ivec4_as_array(v) (&(v)[0])
#define fe_ivec4_as_color(v) ((fe_ivec4_color*)fe_ivec4_as_array(v))
#define fe_ivec4_as_coord(v) ((fe_ivec4_coord*)fe_ivec4_as_array(v))

#define fe_ivec4_add(s,a,b)   ((s)=(a)+(b))
#define fe_ivec4_sub(s,a,b)   ((s)=(a)-(b))
#define fe_ivec4_scale(r,v,s) ((r)=(v)*(s))
#define fe_ivec4_dot(a,b) fe_ivec4_mul_inner(a,b)
static inline int32_t fe_ivec4_mul_inner(const fe_ivec4 a, const fe_ivec4 b) {
    fe_ivec4 v = a*b;
    return v[0]+v[1]+v[2]+v[3];
}
#define fe_ivec4_len(v)  sqrt(fe_ivec4_mul_inner(v, v))
#define fe_ivec4_lenf(v) sqrtf(fe_ivec4_mul_inner(v, v))
#define fe_ivec4_norm(r,v) fe_ivec4_scale(r, v, 1./fe_ivec4_len(v))

/* TODO benchmark me */
#define fe_ivec4_mul_cross(r,a,b) fe_ivec4p_mul_cross(&(r),a,b)
#define fe_ivec4_cross(r,a,b)     fe_ivec4_mul_cross(r,a,b)
static inline void fe_ivec4p_mul_cross(fe_ivec4 *r, const fe_ivec4 a, const fe_ivec4 b) {
    const fe_uvec4 s1 = {1, 2, 0, 0};
    const fe_uvec4 s2 = {2, 0, 1, 0};
    const fe_ivec4 la = fe_ivec4_shuffle(a, s1);
    const fe_ivec4 lb = fe_ivec4_shuffle(b, s2);
    const fe_ivec4 ra = fe_ivec4_shuffle(a, s2);
    const fe_ivec4 rb = fe_ivec4_shuffle(b, s1);
    *r = la*lb - ra*rb;
    (*r)[3] = 1;
}

/* TODO should be discarded if proven to be less efficient. */
#define fe_ivec4_mul_cross_naive(r,a,b) fe_ivec4p_mul_cross_naive(&(r),a,b)
static inline void fe_ivec4p_mul_cross_naive(fe_ivec4 *r, const fe_ivec4 a, const fe_ivec4 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    (*r)[3] = 1;
}

#define fe_ivec4_reflect(r,v,n) fe_ivec4p_reflect(&r,v,n)
static inline void fe_ivec4p_reflect(fe_ivec4 *r, const fe_ivec4 v, const fe_ivec4 n) {
    const int32_t p = 2*fe_ivec4_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_IVEC4_H */
