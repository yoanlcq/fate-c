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
changecom()
/* This file was generated automagically from generator, using Make and 
 * GNU M4. Therefore, it's pointless to edit it. Edit generator instead. 
 *
 * Thanks to Datenwolf for providing his 'linmath.h' library under the WTFPL
 * license, which was used as a reference to implement this file's 
 * functionalities.
 */

define(`VEC',translit(vec, `a-z', `A-Z'))dnl
define(`NS',translit(ns, `a-z', `A-Z'))dnl
dnl
#ifndef PREFIX`'VEC`'_H
#define PREFIX`'VEC`'_H

/* Feature test section */

#ifdef __clang__
#if __has_extension(attribute_ext_vector_type) \
 && __has_builtin(__builtin_shufflevector)
    #define NS`'VEC`'_SIZE_ATTR(n) __attribute__((ext_vector_type(n)))
    #define NS`'VEC`'_PACKED_ATTR  __attribute__((__packed__))
    #define ns`'vec`'_shuffle(v,m) \
                __builtin_shufflevector(v,v,m[0],m[1],m[2],m[3])
    #define ns`'vec`'_shuffle2(u,v,m) \
                __builtin_shufflevector(u,v,m[0],m[1],m[2],m[3])
#endif
#elif defined(__GNUC__)
#if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
    #define NS`'VEC`'_SIZE_ATTR(n) __attribute__((vector_size(n*sizeof(type))))
    #define NS`'VEC`'_PACKED_ATTR  __attribute__((__packed__))
    #define ns`'vec`'_shuffle(v,m)    __builtin_shuffle(v,m)
    #define ns`'vec`'_shuffle2(u,v,m) __builtin_shuffle(u,v,m)
#endif
#endif

#ifndef NS`'VEC`'_SIZE_ATTR
#error The current compiler does not support the required vector extensions. \
       Please fall back to the regular naive implementation. 
#endif

/* Here we go. */

#include <stdint.h>
#include <math.h>
ifelse(type,fe_space_unit,#include <fate/units.h>)
ifelse(eval(dim>=3),1,dnl
#include <dir/mask.h> /* Needed for __builtin_shuffle() */)dnl

ifelse(eval(dim!=3),1,dnl
typedef type ns`'vec NS`'VEC`'_SIZE_ATTR(dim);
,dnl
#include <dir/vec4.h>
typedef ns`'vec4 ns`'vec;
)dnl


struct NS`'VEC`'_PACKED_ATTR ns`'vec`'_color {
    type r;
    type g;
    ifelse(eval(dim>=3),1,type b;,/* No blue component. */)
    ifelse(eval(dim>=4),1,type a;,/* No alpha component. */)
};
typedef struct ns`'vec`'_color ns`'vec`'_color;

struct NS`'VEC`'_PACKED_ATTR ns`'vec`'_coord {
    type x;
    type y;
    ifelse(eval(dim>=3),1,type z;,/* No z component. */)
    ifelse(eval(dim>=4),1,type w;,/* No w component. */)
};
typedef struct ns`'vec`'_coord ns`'vec`'_coord;

#define ns`'vec`'_as_array(v) (&(v)[0])
#define ns`'vec`'_as_color(v) ((ns`'vec`'_color*)ns`'vec`'_as_array(v))
#define ns`'vec`'_as_coord(v) ((ns`'vec`'_coord*)ns`'vec`'_as_array(v))

#define ns`'vec`'_add(s,a,b)   ((s)=(a)+(b))
#define ns`'vec`'_sub(s,a,b)   ((s)=(a)-(b))
#define ns`'vec`'_scale(r,v,s) ((r)=(v)*(s))
#define ns`'vec`'_dot(a,b) ns`'vec`'_mul_inner(a,b)
static inline type ns`'vec`'_mul_inner(const ns`'vec a, const ns`'vec b) {
    ns`'vec v = a*b;
    ifelse(
        dim, 4, return v[0]+v[1]+v[2]+v[3],
        dim, 3, return v[0]+v[1]+v[2],
        dim, 2, return v[0]+v[1],
        type r = 0;
        size_t i;
        for(i=0 ; i<dim ; ++i)
            r += v[i];
        return r
    );
}
#define ns`'vec`'_len(v)  sqrt(ns`'vec`'_mul_inner(v, v))
#define ns`'vec`'_lenf(v) sqrtf(ns`'vec`'_mul_inner(v, v))
#define ns`'vec`'_norm(r,v) ns`'vec`'_scale(r, v, 1./ns`'vec`'_len(v))

ifelse(eval(dim>=3),1,
/* TODO benchmark me */
#define ns`'vec`'_mul_cross(r,a,b) ns`'vec`'p_mul_cross(&(r),a,b)
#define ns`'vec`'_cross(r,a,b)     ns`'vec`'_mul_cross(r,a,b)
static inline void ns`'vec`'p_mul_cross(ns`'vec *r, const ns`'vec a, const ns`'vec b) {
dnl Needs to quote this part because of the commas not inside parentheses.
`    const ns`'mask s1 = {1, 2, 0, 0};
    const ns`'mask s2 = {2, 0, 1, 0};'
    const ns`'vec la = ns`'vec`'_shuffle(a, s1);
    const ns`'vec lb = ns`'vec`'_shuffle(b, s2);
    const ns`'vec ra = ns`'vec`'_shuffle(a, s2);
    const ns`'vec rb = ns`'vec`'_shuffle(b, s1);
    *r = la*lb - ra*rb;
    ifelse(eval(dim==4),1,(*r)[3] = 1;)
}

/* TODO should be discarded if proven to be less efficient. */
#define ns`'vec`'_mul_cross_naive(r,a,b) ns`'vec`'p_mul_cross_naive(&(r),a,b)
static inline void ns`'vec`'p_mul_cross_naive(ns`'vec *r, const ns`'vec a, const ns`'vec b) {
    (*r)[0] = a[1]*b[2] - a[2]*b[1];
    (*r)[1] = a[2]*b[0] - a[0]*b[2];
    (*r)[2] = a[0]*b[1] - a[1]*b[0];
    ifelse(eval(dim==4),1,(*r)[3] = 1;)
}
,/* No cross product for ns`'vec. */
)dnl

#define ns`'vec`'_reflect(r,v,n) ns`'vec`'p_reflect(&r,v,n)
static inline void ns`'vec`'p_reflect(ns`'vec *r, const ns`'vec v, const ns`'vec n) {
    const type p = 2*ns`'vec`'_mul_inner(v, n);
    *r = v-p*n;
}

#endif /* PREFIX`'VEC`'_H */
