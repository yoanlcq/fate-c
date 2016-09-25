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

/*! \file fate/math/ivec3.h
 *  \brief ivec3
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

#ifndef FE_MATH_IVEC3_H
#define FE_MATH_IVEC3_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/ivec3.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/ivec3.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <fate/decl.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO 
 *
 * fe_ivec3
 */
typedef struct { int32_t at[3]; } fe_ivec3;

/*! \brief TODO */
struct FE_DECL_PACKED_STRUCT fe_ivec3_color {
    int32_t r;
    int32_t g;
    int32_t b;
    /* No alpha component. */
};
typedef struct fe_ivec3_color fe_ivec3_color;

/*! \brief TODO */
struct FE_DECL_PACKED_STRUCT fe_ivec3_coord {
    int32_t x;
    int32_t y;
    int32_t z;
    /* No w component. */
};
typedef struct fe_ivec3_coord fe_ivec3_coord;

/*! \brief TODO */
#define fe_ivec3_as_array(v) (v.at)
/*! \brief TODO */
#define fe_ivec3_as_color(v) ((fe_ivec3_color*)fe_ivec3_as_array(v))
/*! \brief TODO */
#define fe_ivec3_as_coord(v) ((fe_ivec3_coord*)fe_ivec3_as_array(v))

/*! \brief TODO */
static inline void fe_ivec3_add(fe_ivec3 * r, const fe_ivec3 * a, const fe_ivec3 * b) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_ivec3_sub(fe_ivec3 * r, const fe_ivec3 * a, const fe_ivec3 * b) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_ivec3_scale(fe_ivec3 * r, const fe_ivec3 * v, const int32_t s) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_ivec3_dot(a,b) fe_ivec3_mul_inner(a,b)
/*! \brief TODO */
static inline int32_t fe_ivec3_mul_inner(const fe_ivec3 * a, const fe_ivec3 * b) {
	int32_t p;
	size_t i;
	for(p=i=0; i<3; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_ivec3_len(v)  sqrt(fe_ivec3_mul_inner(v, v))
/*! \brief TODO */
#define fe_ivec3_lenf(v) sqrtf(fe_ivec3_mul_inner(v, v))
/*! \brief TODO */
#define fe_ivec3_norm(r,v) fe_ivec3_scale(r, v, 1./fe_ivec3_len(v))

/*! \brief TODO */
#define fe_ivec3_cross(r,a,b) fe_ivec3_mul_cross(r,a,b)
/*! \brief TODO */
static inline void fe_ivec3_mul_cross(fe_ivec3 * r, const fe_ivec3 * a, const fe_ivec3 * b) {
	r->at[0] = a->at[1]*b->at[2] - a->at[2]*b->at[1];
	r->at[1] = a->at[2]*b->at[0] - a->at[0]*b->at[2];
	r->at[2] = a->at[0]*b->at[1] - a->at[1]*b->at[0];
	
}

/*! \brief TODO */
static inline void fe_ivec3_reflect(fe_ivec3 * r, const  fe_ivec3 * v, const fe_ivec3 * n) {
	const int32_t p = 2*fe_ivec3_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<3 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_IVEC3_H */
