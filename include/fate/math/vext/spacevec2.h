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

#ifndef FE_MATH_VEXT_SPACEVEC2_H
#define FE_MATH_VEXT_SPACEVEC2_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define SPACEVEC2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define SPACEVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define spacevec2_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define spacevec2_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define SPACEVEC2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(fe_space_unit))))
    #define SPACEVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define spacevec2_shuffle(v,m)    __builtin_shuffle(v,m)
    #define spacevec2_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef SPACEVEC2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
#include <fate/units.h>

typedef fe_space_unit spacevec2 SPACEVEC2_SIZE_ATTR(2);


struct SPACEVEC2_PACKED_ATTR spacevec2_color {
    fe_space_unit r;
    fe_space_unit g;
    /* No blue component. */
    /* No alpha component. */
};
typedef struct spacevec2_color spacevec2_color;

struct SPACEVEC2_PACKED_ATTR spacevec2_coord {
    fe_space_unit x;
    fe_space_unit y;
    /* No z component. */
    /* No w component. */
};
typedef struct spacevec2_coord spacevec2_coord;

#define spacevec2_as_array(v) (&(v)[0])
#define spacevec2_as_color(v) ((spacevec2_color*)spacevec2_as_array(v))
#define spacevec2_as_coord(v) ((spacevec2_coord*)spacevec2_as_array(v))

#define spacevec2_add(s,a,b)   ((s)=(a)+(b))
#define spacevec2_sub(s,a,b)   ((s)=(a)-(b))
#define spacevec2_scale(r,v,s) ((r)=(v)*(s))
#define spacevec2_dot(a,b) spacevec2_mul_inner(a,b)
static inline fe_space_unit spacevec2_mul_inner(const spacevec2 a, const spacevec2 b) {
    spacevec2 v = a*b;
    return v[0]+v[1];
}
#define spacevec2_len(v)  sqrt(spacevec2_mul_inner(v, v))
#define spacevec2_lenf(v) sqrtf(spacevec2_mul_inner(v, v))
#define spacevec2_norm(r,v) spacevec2_scale(r, v, 1./spacevec2_len(v))

/* No cross product for spacevec2. */

#define spacevec2_reflect(r,v,n) spacevec2p_reflect(&r,v,n)
static inline void spacevec2p_reflect(spacevec2 *r, const spacevec2 v, const spacevec2 n) {
    const fe_space_unit p = 2*spacevec2_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_SPACEVEC2_H */
