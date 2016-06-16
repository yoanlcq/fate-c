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

/*! \file fate/math/wvec4.h
 *  \brief wvec4
 *  \ingroup math
 * 
 * This file was generated automagically from genvec.h.m4, using Make and 
 * GNU M4. Therefore, it's pointless to edit it. Edit genvec.h.m4 instead. 
 *
 * Thanks to Datenwolf for providing his 'linmath.h' library under the WTFPL
 * license, which was used as a reference to implement this file's 
 * functionalities.
 *
 * @{
 */

#ifndef FE_MATH_WVEC4_H
#define FE_MATH_WVEC4_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/wvec4.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/wvec4.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <fate/units.h>
/*! \brief TODO 
 *
 * fe_wvec4
 */
typedef struct { fe_space_unit at[4]; } fe_wvec4;

/*! \brief TODO */
struct FE_PACKED_STRUCT fe_wvec4_color {
    fe_space_unit r;
    fe_space_unit g;
    fe_space_unit b;
    fe_space_unit a;
};
typedef struct fe_wvec4_color fe_wvec4_color;

/*! \brief TODO */
struct FE_PACKED_STRUCT fe_wvec4_coord {
    fe_space_unit x;
    fe_space_unit y;
    fe_space_unit z;
    fe_space_unit w;
};
typedef struct fe_wvec4_coord fe_wvec4_coord;

/*! \brief TODO */
#define fe_wvec4_as_array(v) (v.at)
/*! \brief TODO */
#define fe_wvec4_as_color(v) ((fe_wvec4_color*)fe_wvec4_as_array(v))
/*! \brief TODO */
#define fe_wvec4_as_coord(v) ((fe_wvec4_coord*)fe_wvec4_as_array(v))

/*! \brief TODO */
static inline void fe_wvec4_add(fe_wvec4 * r, const fe_wvec4 * a, const fe_wvec4 * b) {
	size_t i;
	for(i=0; i<4; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_wvec4_sub(fe_wvec4 * r, const fe_wvec4 * a, const fe_wvec4 * b) {
	size_t i;
	for(i=0; i<4; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_wvec4_scale(fe_wvec4 * r, const fe_wvec4 * v, const fe_space_unit s) {
	size_t i;
	for(i=0; i<4; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_wvec4_dot(a,b) fe_wvec4_mul_inner(a,b)
/*! \brief TODO */
static inline fe_space_unit fe_wvec4_mul_inner(const fe_wvec4 * a, const fe_wvec4 * b) {
	fe_space_unit p;
	size_t i;
	for(p=i=0; i<4; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_wvec4_len(v)  sqrt(fe_wvec4_mul_inner(v, v))
/*! \brief TODO */
#define fe_wvec4_lenf(v) sqrtf(fe_wvec4_mul_inner(v, v))
/*! \brief TODO */
#define fe_wvec4_norm(r,v) fe_wvec4_scale(r, v, 1./fe_wvec4_len(v))

/*! \brief TODO */
#define fe_wvec4_cross(r,a,b) fe_wvec4_mul_cross(r,a,b)
/*! \brief TODO */
static inline void fe_wvec4_mul_cross(fe_wvec4 * r, const fe_wvec4 * a, const fe_wvec4 * b) {
	r->at[0] = a->at[1]*b->at[2] - a->at[2]*b->at[1];
	r->at[1] = a->at[2]*b->at[0] - a->at[0]*b->at[2];
	r->at[2] = a->at[0]*b->at[1] - a->at[1]*b->at[0];
	r->at[3] = 1;
}

/*! \brief TODO */
static inline void fe_wvec4_reflect(fe_wvec4 * r, const  fe_wvec4 * v, const fe_wvec4 * n) {
	const fe_space_unit p = 2*fe_wvec4_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<4 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_WVEC4_H */
