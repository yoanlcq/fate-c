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
define(`VEC',translit(vec, `a-z', `A-Z'))dnl
define(`NS',translit(ns, `a-z', `A-Z'))dnl
dnl
/*! \file dir/vec.h
 *  \brief vec
 *  \ingroup math
 * 
 * This file was generated automagically from generator, using Make and 
 * GNU M4. Therefore, it's pointless to edit it. Edit generator instead. 
 *
 * Thanks to Datenwolf for providing his 'linmath.h' library under the WTFPL
 * license, which was used as a reference to implement this file's 
 * functionalities.
 *
 * <b>Compile-time switches :</b>
 * <table>
 * <tr>
 * <td>FE_MATH_WITHOUT_VECTOR_EXTENSIONS</td>
 * <td>
 * By default, vector operations are implemented
 * using compiler-specific vector extensions (as of today, GCC and Clang).<br>
 * Defining this macro disables this behaviour and enforces naive implementations.
 * </td>
 * </tr>
 * </table>
 *
 * @{
 */

#ifndef PREFIX`'VEC`'_H
#define PREFIX`'VEC`'_H

#include <fate/defs.h>
#include <fate/decl.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
ifelse(type,fe_space_unit,#include <fate/units.h>)


#ifndef FE_MATH_WITHOUT_VECTOR_EXTENSIONS
    #ifdef __clang__
        #if __has_extension(attribute_ext_vector_type) \
        && __has_builtin(__builtin_shufflevector)
            #include <dir/vext/vec.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
    #elif defined(__GNUC__)
        #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
            #include <dir/vext/vec.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
    #endif
#endif

#if defined(__DOXYGEN__) || defined(FE_MATH_WITHOUT_VECTOR_EXTENSIONS) /* Naive */

/*! \brief TODO 
 *
 * ns`'vec
 */
typedef struct { 
    union {
        type at[dim]; 
        struct {
            type r;
            type g;
            ifelse(eval(dim>=3),1,type b;,/* No blue component. */)
            ifelse(eval(dim>=4),1,type a;,/* No alpha component. */)
        };
        struct {
            type x;
            type y;
            ifelse(eval(dim>=3),1,type z;,/* No z component. */)
            ifelse(eval(dim>=4),1,type w;,/* No w component. */)
        };
        struct {
            type s;
            type t;
            ifelse(eval(dim>=3),1,type p;,/* No p component. */)
            ifelse(eval(dim>=4),1,type q;,/* No q component. */)
        };
    };
} ns`'vec;

fe_static_assert(offsetof(ns`'vec, r) == offsetof(ns`'vec, at[0]), "");
fe_static_assert(offsetof(ns`'vec, g) == offsetof(ns`'vec, at[1]), "");
ifelse(eval(dim>=3),1,fe_static_assert(offsetof(ns`'vec, b) == offsetof(ns`'vec, at[2]), "");)
ifelse(eval(dim>=4),1,fe_static_assert(offsetof(ns`'vec, a) == offsetof(ns`'vec, at[3]), "");)

/*! \brief TODO */
static inline void ns`'vec`'_shuffle(ns`'vec *r, 
                    const ns`'vec *v, 
                    utype m0, utype m1
                    ifelse(eval(dim>=3),1,`,' utype m2)
                    ifelse(eval(dim>=4),1,`,' utype m3)
                    ) {
    ns`'vec tmp = *v; /* Aliasing is allowed */
    r->at[0] = tmp.at[m0%dim];
    r->at[1] = tmp.at[m1%dim];
    ifelse(eval(dim>=3),1, r->at[2] = tmp.at[m2%dim];)
    ifelse(eval(dim>=4),1, r->at[3] = tmp.at[m3%dim];)
}

/*! \brief TODO */
static inline void ns`'vec`'_shuffle2(ns`'vec *r, 
                    const ns`'vec *u,
                    const ns`'vec *v,
                    utype m0, utype m1
                    ifelse(eval(dim>=3),1,`,' utype m2)
                    ifelse(eval(dim>=4),1,`,' utype m3)
                    ) {
    type ary[2*dim] = {
        u->at[0], u->at[1]
        ifelse(eval(dim>=3),1,`,' u->at[2])
        ifelse(eval(dim>=4),1,`,' u->at[3])  
        , v->at[0], v->at[1]
        ifelse(eval(dim>=3),1,`,' v->at[2])
        ifelse(eval(dim>=4),1,`,' v->at[3])  
    };
    r->at[0] = ary[m0%(2*dim)];
    r->at[1] = ary[m1%(2*dim)];
    ifelse(eval(dim>=3),1, r->at[2] = ary[m2%(2*dim)];)
    ifelse(eval(dim>=4),1, r->at[3] = ary[m3%(2*dim)];)
}


/*! \brief TODO */
static inline void ns`'vec`'_add(ns`'vec * r, const ns`'vec * a, const ns`'vec * b) {
	size_t i;
	for(i=0; i<dim; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void ns`'vec`'_sub(ns`'vec * r, const ns`'vec * a, const ns`'vec * b) {
	size_t i;
	for(i=0; i<dim; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void ns`'vec`'_scale(ns`'vec * r, const ns`'vec * v, const type s) {
	size_t i;
	for(i=0; i<dim; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define ns`'vec`'_dot(a,b) ns`'vec`'_mul_inner(a,b)
/*! \brief TODO */
static inline type ns`'vec`'_mul_inner(const ns`'vec * a, const ns`'vec * b) {
	type p;
	size_t i;
	for(p=i=0; i<dim; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define ns`'vec`'_len(v)  sqrt(ns`'vec`'_mul_inner(v, v))
/*! \brief TODO */
#define ns`'vec`'_lenf(v) sqrtf(ns`'vec`'_mul_inner(v, v))
/*! \brief TODO */
#define ns`'vec`'_norm(r,v) ns`'vec`'_scale(r, v, 1./ns`'vec`'_len(v))

ifelse(eval(dim>=3),1,
/*! \brief TODO */
#define ns`'vec`'_cross(r,a,b) ns`'vec`'_mul_cross(r,a,b)
/*! \brief TODO */
static inline void ns`'vec`'_mul_cross(ns`'vec * r, const ns`'vec * a, const ns`'vec * b) {
	r->at[0] = a->at[1]*b->at[2] - a->at[2]*b->at[1];
	r->at[1] = a->at[2]*b->at[0] - a->at[0]*b->at[2];
	r->at[2] = a->at[0]*b->at[1] - a->at[1]*b->at[0];
	ifelse(eval(dim>=4),1,r->at[3] = 1;)
}
,/* No cross product for ns`'vec. */
)dnl

/*! \brief TODO */
static inline void ns`'vec`'_reflect(ns`'vec * r, const  ns`'vec * v, const ns`'vec * n) {
	const type p = 2*ns`'vec`'_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<dim ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* PREFIX`'VEC`'_H */
