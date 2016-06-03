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

#ifndef FE_MATH_VEXT_IVEC2_H
#define FE_MATH_VEXT_IVEC2_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define IVEC2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define IVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define ivec2_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define ivec2_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define IVEC2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(int32_t))))
    #define IVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define ivec2_shuffle(v,m)    __builtin_shuffle(v,m)
    #define ivec2_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef IVEC2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>


typedef int32_t ivec2 IVEC2_SIZE_ATTR(2);


struct IVEC2_PACKED_ATTR ivec2_color {
    int32_t r;
    int32_t g;
    /* No blue component. */
    /* No alpha component. */
};
typedef struct ivec2_color ivec2_color;

struct IVEC2_PACKED_ATTR ivec2_coord {
    int32_t x;
    int32_t y;
    /* No z component. */
    /* No w component. */
};
typedef struct ivec2_coord ivec2_coord;

#define ivec2_as_array(v) (&(v)[0])
#define ivec2_as_color(v) ((ivec2_color*)ivec2_as_array(v))
#define ivec2_as_coord(v) ((ivec2_coord*)ivec2_as_array(v))

#define ivec2_add(s,a,b)   ((s)=(a)+(b))
#define ivec2_sub(s,a,b)   ((s)=(a)-(b))
#define ivec2_scale(r,v,s) ((r)=(v)*(s))
#define ivec2_dot(a,b) ivec2_mul_inner(a,b)
static inline int32_t ivec2_mul_inner(const ivec2 a, const ivec2 b) {
    ivec2 v = a*b;
    return v[0]+v[1];
}
#define ivec2_len(v)  sqrt(ivec2_mul_inner(v, v))
#define ivec2_lenf(v) sqrtf(ivec2_mul_inner(v, v))
#define ivec2_norm(r,v) ivec2_scale(r, v, 1./ivec2_len(v))

/* No cross product for ivec2. */

#define ivec2_reflect(r,v,n) ivec2p_reflect(&r,v,n)
static inline void ivec2p_reflect(ivec2 *r, const ivec2 v, const ivec2 n) {
    const int32_t p = 2*ivec2_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_IVEC2_H */
