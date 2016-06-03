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

/* This file was generated automagically from genvec.h.m4, using Make and 
 * GNU M4. Therefore, it's pointless to edit it. Edit genvec.h.m4 instead. 
 *
 * Thanks to Datenwolf for providing his 'linmath.h' library under the WTFPL
 * license, which was used as a reference to implement this file's 
 * functionalities.
 */

/*! \file fate/math/dvec4.h
 *  \brief dvec4
 *  \ingroup math
 * 
 * TODO
 *
 * @{
 */

#ifndef FE_MATH_DVEC4_H
#define FE_MATH_DVEC4_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/dvec4.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/dvec4.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO 
 *
 * dvec4
 */
typedef double dvec4[4];

/*! \brief TODO */
struct FE_PACKED_STRUCT dvec4_color {
    double r;
    double g;
    double b;
    double a;
};
typedef struct dvec4_color dvec4_color;

/*! \brief TODO */
struct FE_PACKED_STRUCT dvec4_coord {
    double x;
    double y;
    double z;
    double w;
};
typedef struct dvec4_coord dvec4_coord;

/*! \brief TODO */
#define dvec4_as_array(v) (v)
/*! \brief TODO */
#define dvec4_as_color(v) ((dvec4_color*)dvec4_as_array(v))
/*! \brief TODO */
#define dvec4_as_coord(v) ((dvec4_coord*)dvec4_as_array(v))

/*! \brief TODO */
static inline void dvec4_add(dvec4 r, const dvec4 a, const dvec4 b) {
	size_t i;
	for(i=0; i<4; ++i)
		r[i] = a[i] + b[i];
}
/*! \brief TODO */
static inline void dvec4_sub(dvec4 r, const dvec4 a, const dvec4 b) {
	size_t i;
	for(i=0; i<4; ++i)
		r[i] = a[i] - b[i];
}
/*! \brief TODO */
static inline void dvec4_scale(dvec4 r, const dvec4 v, const double s) {
	size_t i;
	for(i=0; i<4; ++i)
		r[i] = v[i] * s;
}

/*! \brief TODO */
#define dvec4_dot(a,b) dvec4_mul_inner(a,b)
/*! \brief TODO */
static inline double dvec4_mul_inner(const dvec4 a, const dvec4 b) {
	double p;
	size_t i;
	for(p=i=0; i<4; ++i)
		p += b[i]*a[i];
	return p;
}

/*! \brief TODO */
#define dvec4_len(v)  sqrt(dvec4_mul_inner(v, v))
/*! \brief TODO */
#define dvec4_lenf(v) sqrtf(dvec4_mul_inner(v, v))
/*! \brief TODO */
#define dvec4_norm(r,v) dvec4_scale(r, v, 1./dvec4_len(v))

/*! \brief TODO */
#define dvec4_cross(r,a,b) dvec4_mul_cross(r,a,b)
/*! \brief TODO */
static inline void dvec4_mul_cross(dvec4 r, const dvec4 a, const dvec4 b) {
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
	r[3] = 1.f;
}

/*! \brief TODO */
static inline void dvec4_reflect(dvec4 r, const dvec4 v, const dvec4 n) {
	const double p = 2*dvec4_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<4 ; ++i)
		r[i] = v[i] - p*n[i];
}

#endif /* Naive */

#endif /* FE_MATH_DVEC4_H */

/* @} */
