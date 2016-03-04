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
dnl
/*! \file dir/vec.h
 *  \brief vec
 *  \ingroup math
 * 
 * TODO
 *
 * @{
 */

#ifndef prefix`'VEC`'suffix`'_H
#define prefix`'VEC`'suffix`'_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <dir/vext/vec.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <dir/vext/vec.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
ifelse(type,fate_space_unit,#include <fate/units.h>)
/*! \brief TODO */
typedef type vec[dim];

/*! \brief TODO */
struct FATE_PACKED_STRUCT vec`'_color {
    type r;
    type g;
    ifelse(eval(dim>=3),1,type b;,/* No blue component. */)
    ifelse(eval(dim>=4),1,type a;,/* No alpha component. */)
};
typedef struct vec`'_color vec`'_color;

/*! \brief TODO */
struct FATE_PACKED_STRUCT vec`'_coord {
    type x;
    type y;
    ifelse(eval(dim>=3),1,type z;,/* No z component. */)
    ifelse(eval(dim>=4),1,type w;,/* No w component. */)
};
typedef struct vec`'_coord vec`'_coord;

/*! \brief TODO */
#define vec`'_as_array(v) (v)
/*! \brief TODO */
#define vec`'_as_color(v) ((vec`'_color*)vec`'_as_array(v))
/*! \brief TODO */
#define vec`'_as_coord(v) ((vec`'_coord*)vec`'_as_array(v))

/*! \brief TODO */
static inline void vec`'_add(vec r, const vec a, const vec b) {
	size_t i;
	for(i=0; i<dim; ++i)
		r[i] = a[i] + b[i];
}
/*! \brief TODO */
static inline void vec`'_sub(vec r, const vec a, const vec b) {
	size_t i;
	for(i=0; i<dim; ++i)
		r[i] = a[i] - b[i];
}
/*! \brief TODO */
static inline void vec`'_scale(vec r, const vec v, const type s) {
	size_t i;
	for(i=0; i<dim; ++i)
		r[i] = v[i] * s;
}

/*! \brief TODO */
#define vec`'_dot(a,b) vec`'_mul_inner(a,b)
/*! \brief TODO */
static inline type vec`'_mul_inner(const vec a, const vec b) {
	type p;
	size_t i;
	for(p=i=0; i<dim; ++i)
		p += b[i]*a[i];
	return p;
}

/*! \brief TODO */
#define vec`'_len(v)  sqrt(vec`'_mul_inner(v, v))
/*! \brief TODO */
#define vec`'_lenf(v) sqrtf(vec`'_mul_inner(v, v))
/*! \brief TODO */
#define vec`'_norm(r,v) vec`'_scale(r, v, 1./vec`'_len(v))

ifelse(eval(dim>=3),1,
/*! \brief TODO */
#define vec`'_cross(r,a,b) vec`'_mul_cross(r,a,b)
/*! \brief TODO */
static inline void vec`'_mul_cross(vec r, const vec a, const vec b) {
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
	ifelse(eval(dim>=4),1,r[3] = 1.f;)
}
,/* No cross product for vec. */
)dnl

/*! \brief TODO */
static inline void vec`'_reflect(vec r, const vec v, const vec n) {
	const type p = 2*vec`'_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<dim ; ++i)
		r[i] = v[i] - p*n[i];
}

#endif /* Naive */

#endif /* prefix`'VEC`'suffix`'_H */

/* @} */
