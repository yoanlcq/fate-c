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

#ifndef FE_MATH_VEXT_UVEC2_H
#define FE_MATH_VEXT_UVEC2_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_UVEC2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_UVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_uvec2_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define fe_uvec2_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_UVEC2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(uint32_t))))
    #define FE_UVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_uvec2_shuffle(v,m)    __builtin_shuffle(v,m)
    #define fe_uvec2_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef FE_UVEC2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>


typedef uint32_t fe_uvec2 FE_UVEC2_SIZE_ATTR(2);


struct FE_UVEC2_PACKED_ATTR fe_uvec2_color {
    uint32_t r;
    uint32_t g;
    /* No blue component. */
    /* No alpha component. */
};
typedef struct fe_uvec2_color fe_uvec2_color;

struct FE_UVEC2_PACKED_ATTR fe_uvec2_coord {
    uint32_t x;
    uint32_t y;
    /* No z component. */
    /* No w component. */
};
typedef struct fe_uvec2_coord fe_uvec2_coord;

#define fe_uvec2_as_array(v) (&(v)[0])
#define fe_uvec2_as_color(v) ((fe_uvec2_color*)fe_uvec2_as_array(v))
#define fe_uvec2_as_coord(v) ((fe_uvec2_coord*)fe_uvec2_as_array(v))

#define fe_uvec2_add(s,a,b)   ((s)=(a)+(b))
#define fe_uvec2_sub(s,a,b)   ((s)=(a)-(b))
#define fe_uvec2_scale(r,v,s) ((r)=(v)*(s))
#define fe_uvec2_dot(a,b) fe_uvec2_mul_inner(a,b)
static inline uint32_t fe_uvec2_mul_inner(const fe_uvec2 a, const fe_uvec2 b) {
    fe_uvec2 v = a*b;
    return v[0]+v[1];
}
#define fe_uvec2_len(v)  sqrt(fe_uvec2_mul_inner(v, v))
#define fe_uvec2_lenf(v) sqrtf(fe_uvec2_mul_inner(v, v))
#define fe_uvec2_norm(r,v) fe_uvec2_scale(r, v, 1./fe_uvec2_len(v))

/* No cross product for fe_uvec2. */

#define fe_uvec2_reflect(r,v,n) fe_uvec2p_reflect(&r,v,n)
static inline void fe_uvec2p_reflect(fe_uvec2 *r, const fe_uvec2 v, const fe_uvec2 n) {
    const uint32_t p = 2*fe_uvec2_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_UVEC2_H */
