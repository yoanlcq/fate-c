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

#ifndef FATE_MATH_VEXT_UVEC3_H
#define FATE_MATH_VEXT_UVEC3_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define UVEC3_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define UVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define uvec3_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define uvec3_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define UVEC3_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(uint32_t))))
    #define UVEC3_PACKED_ATTR  __attribute__((__packed__))
    #define uvec3_shuffle(v,m)    __builtin_shuffle(v,m)
    #define uvec3_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef UVEC3_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>

#include <fate/math/vext/uvec4.h> /* Needed for __builtin_shuffle() */
#include <fate/math/vext/uvec4.h>
typedef uvec4 uvec3;


struct UVEC3_PACKED_ATTR uvec3_color {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    /* No alpha component. */
};
typedef struct uvec3_color uvec3_color;

struct UVEC3_PACKED_ATTR uvec3_coord {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    /* No w component. */
};
typedef struct uvec3_coord uvec3_coord;

#define uvec3_as_array(v) (&(v)[0])
#define uvec3_as_color(v) ((uvec3_color*)uvec3_as_array(v))
#define uvec3_as_coord(v) ((uvec3_coord*)uvec3_as_array(v))

#define uvec3_add(s,a,b)   ((s)=(a)+(b))
#define uvec3_sub(s,a,b)   ((s)=(a)-(b))
#define uvec3_scale(r,v,s) ((r)=(v)*(s))
#define uvec3_dot(a,b) uvec3_mul_inner(a,b)
static inline uint32_t uvec3_mul_inner(const uvec3 a, const uvec3 b) {
    uvec3 v = a*b;
    return v[0]+v[1]+v[2];
}
#define uvec3_len(v)  sqrt(uvec3_mul_inner(v, v))
#define uvec3_lenf(v) sqrtf(uvec3_mul_inner(v, v))
#define uvec3_norm(r,v) uvec3_scale(r, v, 1./uvec3_len(v))

/* TODO benchmark me */
#define uvec3_mul_cross(r,a,b) uvec3p_mul_cross(&(r),a,b)
#define uvec3_cross(r,a,b)     uvec3_mul_cross(r,a,b)
static inline void uvec3p_mul_cross(uvec3 *r, const uvec3 a, const uvec3 b) {
    const uvec4 s1 = {1, 2, 0, 0};
    const uvec4 s2 = {2, 0, 1, 0};
    const uvec3 la = uvec3_shuffle(a, s1);
    const uvec3 lb = uvec3_shuffle(b, s2);
    const uvec3 ra = uvec3_shuffle(a, s2);
    const uvec3 rb = uvec3_shuffle(b, s1);
    *r = la*lb - ra*rb;
    
}

/* TODO should be discarded if proven to be less efficient. */
#define uvec3_mul_cross_naive(r,a,b) uvec3p_mul_cross_naive(&(r),a,b)
static inline void uvec3p_mul_cross_naive(uvec3 *r, const uvec3 a, const uvec3 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    
}

#define uvec3_reflect(r,v,n) uvec3p_reflect(&r,v,n)
static inline void uvec3p_reflect(uvec3 *r, const uvec3 v, const uvec3 n) {
    const uint32_t p = 2*uvec3_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FATE_MATH_VEXT_UVEC3_H */
