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

#ifndef FATE_MATH_VEXT_UVEC4_H
#define FATE_MATH_VEXT_UVEC4_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define UVEC4_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define UVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define uvec4_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define uvec4_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define UVEC4_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(uint32_t))))
    #define UVEC4_PACKED_ATTR  __attribute__((__packed__))
    #define uvec4_shuffle(v,m)    __builtin_shuffle(v,m)
    #define uvec4_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef UVEC4_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>

#include <fate/math/vext/uvec4.h> /* Needed for __builtin_shuffle() */
typedef uint32_t uvec4 UVEC4_SIZE_ATTR(4);


struct UVEC4_PACKED_ATTR uvec4_color {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
};
typedef struct uvec4_color uvec4_color;

struct UVEC4_PACKED_ATTR uvec4_coord {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t w;
};
typedef struct uvec4_coord uvec4_coord;

#define uvec4_as_array(v) (&(v)[0])
#define uvec4_as_color(v) ((uvec4_color*)uvec4_as_array(v))
#define uvec4_as_coord(v) ((uvec4_coord*)uvec4_as_array(v))

#define uvec4_add(s,a,b)   ((s)=(a)+(b))
#define uvec4_sub(s,a,b)   ((s)=(a)-(b))
#define uvec4_scale(r,v,s) ((r)=(v)*(s))
#define uvec4_dot(a,b) uvec4_mul_inner(a,b)
static inline uint32_t uvec4_mul_inner(const uvec4 a, const uvec4 b) {
    uvec4 v = a*b;
    return v[0]+v[1]+v[2]+v[3];
}
#define uvec4_len(v)  sqrt(uvec4_mul_inner(v, v))
#define uvec4_lenf(v) sqrtf(uvec4_mul_inner(v, v))
#define uvec4_norm(r,v) uvec4_scale(r, v, 1./uvec4_len(v))

/* TODO benchmark me */
#define uvec4_mul_cross(r,a,b) uvec4p_mul_cross(&(r),a,b)
#define uvec4_cross(r,a,b)     uvec4_mul_cross(r,a,b)
static inline void uvec4p_mul_cross(uvec4 *r, const uvec4 a, const uvec4 b) {
    const uvec4 s1 = {1, 2, 0, 0};
    const uvec4 s2 = {2, 0, 1, 0};
    const uvec4 la = uvec4_shuffle(a, s1);
    const uvec4 lb = uvec4_shuffle(b, s2);
    const uvec4 ra = uvec4_shuffle(a, s2);
    const uvec4 rb = uvec4_shuffle(b, s1);
    *r = la*lb - ra*rb;
    (*r)[3] = 1;
}

/* TODO should be discarded if proven to be less efficient. */
#define uvec4_mul_cross_naive(r,a,b) uvec4p_mul_cross_naive(&(r),a,b)
static inline void uvec4p_mul_cross_naive(uvec4 *r, const uvec4 a, const uvec4 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    (*r)[3] = 1;
}

#define uvec4_reflect(r,v,n) uvec4p_reflect(&r,v,n)
static inline void uvec4p_reflect(uvec4 *r, const uvec4 v, const uvec4 n) {
    const uint32_t p = 2*uvec4_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FATE_MATH_VEXT_UVEC4_H */
