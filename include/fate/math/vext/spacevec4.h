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

#ifndef FE_MATH_VEXT_SPACEVEC4_H
#define FE_MATH_VEXT_SPACEVEC4_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define SPACEVEC4_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define SPACEVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define spacevec4_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define spacevec4_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define SPACEVEC4_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(fe_space_unit))))
    #define SPACEVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define spacevec4_shuffle(v,m)    __builtin_shuffle(v,m)
    #define spacevec4_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef SPACEVEC4_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
#include <fate/units.h>
#include <fate/math/vext/uvec4.h> /* Needed for __builtin_shuffle() */
typedef fe_space_unit spacevec4 SPACEVEC4_SIZE_ATTR(4);


struct SPACEVEC4_PACKED_ATTR spacevec4_color {
    fe_space_unit r;
    fe_space_unit g;
    fe_space_unit b;
    fe_space_unit a;
};
typedef struct spacevec4_color spacevec4_color;

struct SPACEVEC4_PACKED_ATTR spacevec4_coord {
    fe_space_unit x;
    fe_space_unit y;
    fe_space_unit z;
    fe_space_unit w;
};
typedef struct spacevec4_coord spacevec4_coord;

#define spacevec4_as_array(v) (&(v)[0])
#define spacevec4_as_color(v) ((spacevec4_color*)spacevec4_as_array(v))
#define spacevec4_as_coord(v) ((spacevec4_coord*)spacevec4_as_array(v))

#define spacevec4_add(s,a,b)   ((s)=(a)+(b))
#define spacevec4_sub(s,a,b)   ((s)=(a)-(b))
#define spacevec4_scale(r,v,s) ((r)=(v)*(s))
#define spacevec4_dot(a,b) spacevec4_mul_inner(a,b)
static inline fe_space_unit spacevec4_mul_inner(const spacevec4 a, const spacevec4 b) {
    spacevec4 v = a*b;
    return v[0]+v[1]+v[2]+v[3];
}
#define spacevec4_len(v)  sqrt(spacevec4_mul_inner(v, v))
#define spacevec4_lenf(v) sqrtf(spacevec4_mul_inner(v, v))
#define spacevec4_norm(r,v) spacevec4_scale(r, v, 1./spacevec4_len(v))

/* TODO benchmark me */
#define spacevec4_mul_cross(r,a,b) spacevec4p_mul_cross(&(r),a,b)
#define spacevec4_cross(r,a,b)     spacevec4_mul_cross(r,a,b)
static inline void spacevec4p_mul_cross(spacevec4 *r, const spacevec4 a, const spacevec4 b) {
    const uvec4 s1 = {1, 2, 0, 0};
    const uvec4 s2 = {2, 0, 1, 0};
    const spacevec4 la = spacevec4_shuffle(a, s1);
    const spacevec4 lb = spacevec4_shuffle(b, s2);
    const spacevec4 ra = spacevec4_shuffle(a, s2);
    const spacevec4 rb = spacevec4_shuffle(b, s1);
    *r = la*lb - ra*rb;
    (*r)[3] = 1;
}

/* TODO should be discarded if proven to be less efficient. */
#define spacevec4_mul_cross_naive(r,a,b) spacevec4p_mul_cross_naive(&(r),a,b)
static inline void spacevec4p_mul_cross_naive(spacevec4 *r, const spacevec4 a, const spacevec4 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    (*r)[3] = 1;
}

#define spacevec4_reflect(r,v,n) spacevec4p_reflect(&r,v,n)
static inline void spacevec4p_reflect(spacevec4 *r, const spacevec4 v, const spacevec4 n) {
    const fe_space_unit p = 2*spacevec4_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_SPACEVEC4_H */
