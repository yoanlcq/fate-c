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

#ifndef FATE_MATH_VEXT_VEC3_H
#define FATE_MATH_VEXT_VEC3_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define VEC3_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define VEC3_PACKED_ATTR  __attribute__((__packed__))
    #define vec3_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define vec3_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define VEC3_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(float))))
    #define VEC3_PACKED_ATTR  __attribute__((__packed__))
    #define vec3_shuffle(v,m)    __builtin_shuffle(v,m)
    #define vec3_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef VEC3_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>

#include <fate/math/vext/uvec4.h> /* Needed for __builtin_shuffle() */
#include <fate/math/vext/vec4.h>
typedef vec4 vec3;


struct VEC3_PACKED_ATTR vec3_color {
    float r;
    float g;
    float b;
    /* No alpha component. */
};
typedef struct vec3_color vec3_color;

struct VEC3_PACKED_ATTR vec3_coord {
    float x;
    float y;
    float z;
    /* No w component. */
};
typedef struct vec3_coord vec3_coord;

#define vec3_as_array(v) (&(v)[0])
#define vec3_as_color(v) ((vec3_color*)vec3_as_array(v))
#define vec3_as_coord(v) ((vec3_coord*)vec3_as_array(v))

#define vec3_add(s,a,b)   ((s)=(a)+(b))
#define vec3_sub(s,a,b)   ((s)=(a)-(b))
#define vec3_scale(r,v,s) ((r)=(v)*(s))
#define vec3_dot(a,b) vec3_mul_inner(a,b)
static inline float vec3_mul_inner(const vec3 a, const vec3 b) {
    vec3 v = a*b;
    return v[0]+v[1]+v[2];
}
#define vec3_len(v)  sqrt(vec3_mul_inner(v, v))
#define vec3_lenf(v) sqrtf(vec3_mul_inner(v, v))
#define vec3_norm(r,v) vec3_scale(r, v, 1./vec3_len(v))

/* TODO benchmark me */
#define vec3_mul_cross(r,a,b) vec3p_mul_cross(&(r),a,b)
#define vec3_cross(r,a,b)     vec3_mul_cross(r,a,b)
static inline void vec3p_mul_cross(vec3 *r, const vec3 a, const vec3 b) {
    const uvec4 s1 = {1, 2, 0, 0};
    const uvec4 s2 = {2, 0, 1, 0};
    const vec3 la = vec3_shuffle(a, s1);
    const vec3 lb = vec3_shuffle(b, s2);
    const vec3 ra = vec3_shuffle(a, s2);
    const vec3 rb = vec3_shuffle(b, s1);
    *r = la*lb - ra*rb;
    
}

/* TODO should be discarded if proven to be less efficient. */
#define vec3_mul_cross_naive(r,a,b) vec3p_mul_cross_naive(&(r),a,b)
static inline void vec3p_mul_cross_naive(vec3 *r, const vec3 a, const vec3 b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    
}

#define vec3_reflect(r,v,n) vec3p_reflect(&r,v,n)
static inline void vec3p_reflect(vec3 *r, const vec3 v, const vec3 n) {
    const float p = 2*vec3_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FATE_MATH_VEXT_VEC3_H */
