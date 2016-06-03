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

#ifndef FE_MATH_VEXT_VEC2_H
#define FE_MATH_VEXT_VEC2_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define VEC2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define VEC2_PACKED_ATTR  __attribute__((__packed__))
    #define vec2_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define vec2_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define VEC2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(float))))
    #define VEC2_PACKED_ATTR  __attribute__((__packed__))
    #define vec2_shuffle(v,m)    __builtin_shuffle(v,m)
    #define vec2_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef VEC2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>


typedef float vec2 VEC2_SIZE_ATTR(2);


struct VEC2_PACKED_ATTR vec2_color {
    float r;
    float g;
    /* No blue component. */
    /* No alpha component. */
};
typedef struct vec2_color vec2_color;

struct VEC2_PACKED_ATTR vec2_coord {
    float x;
    float y;
    /* No z component. */
    /* No w component. */
};
typedef struct vec2_coord vec2_coord;

#define vec2_as_array(v) (&(v)[0])
#define vec2_as_color(v) ((vec2_color*)vec2_as_array(v))
#define vec2_as_coord(v) ((vec2_coord*)vec2_as_array(v))

#define vec2_add(s,a,b)   ((s)=(a)+(b))
#define vec2_sub(s,a,b)   ((s)=(a)-(b))
#define vec2_scale(r,v,s) ((r)=(v)*(s))
#define vec2_dot(a,b) vec2_mul_inner(a,b)
static inline float vec2_mul_inner(const vec2 a, const vec2 b) {
    vec2 v = a*b;
    return v[0]+v[1];
}
#define vec2_len(v)  sqrt(vec2_mul_inner(v, v))
#define vec2_lenf(v) sqrtf(vec2_mul_inner(v, v))
#define vec2_norm(r,v) vec2_scale(r, v, 1./vec2_len(v))

/* No cross product for vec2. */

#define vec2_reflect(r,v,n) vec2p_reflect(&r,v,n)
static inline void vec2p_reflect(vec2 *r, const vec2 v, const vec2 n) {
    const float p = 2*vec2_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_VEC2_H */
