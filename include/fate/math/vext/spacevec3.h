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

#ifndef FATE_MATH_VEXT_SPACEVEC3_H
#define FATE_MATH_VEXT_SPACEVEC3_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define SPACEVEC3_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define SPACEVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define spacevec3_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define spacevec3_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define SPACEVEC3_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(fate_space_unit))))
    #define SPACEVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define spacevec3_shuffle(v,m)    __builtin_shuffle(v,m)
    #define spacevec3_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef SPACEVEC3_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
#include <fate/units.h>
#include <fate/math/vext/uvec4.h> /* Needed for __builtin_shuffle() */
#include <fate/math/vext/spacevec4.h>
typedef spacevec4 spacevec3;


struct SPACEVEC3_PACKED_ATTR spacevec3_color {
    fate_space_unit r;
    fate_space_unit g;
    fate_space_unit b;
    /* No alpha component. */
};
typedef struct spacevec3_color spacevec3_color;

struct SPACEVEC3_PACKED_ATTR spacevec3_coord {
    fate_space_unit x;
    fate_space_unit y;
    fate_space_unit z;
    /* No w component. */
};
typedef struct spacevec3_coord spacevec3_coord;

#define spacevec3_as_array(v) (&(v)[0])
#define spacevec3_as_color(v) ((spacevec3_color*)spacevec3_as_array(v))
#define spacevec3_as_coord(v) ((spacevec3_coord*)spacevec3_as_array(v))

#define spacevec3_add(s,a,b)   ((s)=(a)+(b))
#define spacevec3_sub(s,a,b)   ((s)=(a)-(b))
#define spacevec3_scale(r,v,s) ((r)=(v)*(s))
#define spacevec3_dot(a,b) spacevec3_mul_inner(a,b)
static inline fate_space_unit spacevec3_mul_inner(const spacevec3 a, const spacevec3 b) {
    spacevec3 v = a*b;
    return v[0]+v[1]+v[2];
}
#define spacevec3_len(v)  sqrt(spacevec3_mul_inner(v, v))
#define spacevec3_lenf(v) sqrtf(spacevec3_mul_inner(v, v))
#define spacevec3_norm(r,v) spacevec3_scale(r, v, 1./spacevec3_len(v))

/* TODO benchmark me */
#define spacevec3_mul_cross(r,a,b) spacevec3p_mul_cross(&(r),a,b)
#define spacevec3_cross(r,a,b)     spacevec3_mul_cross(r,a,b)
static inline void spacevec3p_mul_cross(spacevec3 *r, const spacevec3 a, const spacevec3 b) {
    const uvec4 s1 = {1, 2, 0, 0};
    const uvec4 s2 = {2, 0, 1, 0};
    const spacevec3 la = spacevec3_shuffle(a, s1);
    const spacevec3 lb = spacevec3_shuffle(b, s2);
    const spacevec3 ra = spacevec3_shuffle(a, s2);
    const spacevec3 rb = spacevec3_shuffle(b, s1);
    *r = la*lb - ra*rb;
    
}

/* TODO should be discarded if proven to be less efficient. */
#define spacevec3_mul_cross_naive(r,a,b) spacevec3p_mul_cross_naive(&(r),a,b)
static inline void spacevec3p_mul_cross_naive(spacevec3 *r, const spacevec3 a, const spacevec3 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    
}

#define spacevec3_reflect(r,v,n) spacevec3p_reflect(&r,v,n)
static inline void spacevec3p_reflect(spacevec3 *r, const spacevec3 v, const spacevec3 n) {
    const fate_space_unit p = 2*spacevec3_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FATE_MATH_VEXT_SPACEVEC3_H */
