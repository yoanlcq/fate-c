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

#ifndef FE_MATH_VEXT_WVEC2_H
#define FE_MATH_VEXT_WVEC2_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define FE_WVEC2_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define FE_WVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_wvec2_shuffle(v,a,b,c,d) \
                __builtin_shufflevector(v,v,a,b,c,d)
    #define fe_wvec2_shuffle2(u,v,a,b,c,d) \
                __builtin_shufflevector(u,v,a,b,c,d)
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define FE_WVEC2_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(fe_space_unit))))
    #define FE_WVEC2_PACKED_ATTR  __attribute__((__packed__))
    #define fe_wvec2_shuffle(v,a,b,c,d)    __builtin_shuffle(v,(fe_mask){a,b,c,d})
    #define fe_wvec2_shuffle2(u,v,a,b,c,d) __builtin_shuffle(u,v,(fe_mask){a,b,c,d})
#endif
#endif

#ifndef FE_WVEC2_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
#include <fate/units.h>

typedef fe_space_unit fe_wvec2 FE_WVEC2_SIZE_ATTR(2);


struct FE_WVEC2_PACKED_ATTR fe_wvec2_color {
    fe_space_unit r;
    fe_space_unit g;
    /* No blue component. */
    /* No alpha component. */
};
typedef struct fe_wvec2_color fe_wvec2_color;

struct FE_WVEC2_PACKED_ATTR fe_wvec2_coord {
    fe_space_unit x;
    fe_space_unit y;
    /* No z component. */
    /* No w component. */
};
typedef struct fe_wvec2_coord fe_wvec2_coord;

#define fe_wvec2_as_array(v) (&(v)[0])
#define fe_wvec2_as_color(v) ((fe_wvec2_color*)fe_wvec2_as_array(v))
#define fe_wvec2_as_coord(v) ((fe_wvec2_coord*)fe_wvec2_as_array(v))

#define fe_wvec2_add(s,a,b)   ((s)=(a)+(b))
#define fe_wvec2_sub(s,a,b)   ((s)=(a)-(b))
#define fe_wvec2_scale(r,v,s) ((r)=(v)*(s))
#define fe_wvec2_dot(a,b) fe_wvec2_mul_inner(&a,&b)
static inline fe_space_unit fe_wvec2_mul_inner(const fe_wvec2 *a, const fe_wvec2 *b) {
    fe_wvec2 v = (*a)*(*b);
    return v[0]+v[1];
}
#define fe_wvec2_len(v)  sqrt(fe_wvec2_mul_inner(v, v))
#define fe_wvec2_lenf(v) sqrtf(fe_wvec2_mul_inner(v, v))
#define fe_wvec2_norm(r,v) fe_wvec2_scale(r, v, 1./fe_wvec2_len(v))

/* No cross product for fe_wvec2. */

#define fe_wvec2_reflect(r,v,n) fe_wvec2p_reflect(&r,v,n)
static inline void fe_wvec2p_reflect(fe_wvec2 *r, const fe_wvec2 v, const fe_wvec2 n) {
    const fe_space_unit p = 2*fe_wvec2_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* FE_MATH_VEXT_WVEC2_H */
