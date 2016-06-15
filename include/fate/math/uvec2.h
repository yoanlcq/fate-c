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

/*! \file fate/math/uvec2.h
 *  \brief uvec2
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

#ifndef FE_MATH_UVEC2_H
#define FE_MATH_UVEC2_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/uvec2.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/uvec2.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO 
 *
 * fe_uvec2
 */
typedef struct { uint32_t at[2]; } fe_uvec2;

/*! \brief TODO */
struct FATE_PACKED_STRUCT fe_uvec2_color {
    uint32_t r;
    uint32_t g;
    /* No blue component. */
    /* No alpha component. */
};
typedef struct fe_uvec2_color fe_uvec2_color;

/*! \brief TODO */
struct FATE_PACKED_STRUCT fe_uvec2_coord {
    uint32_t x;
    uint32_t y;
    /* No z component. */
    /* No w component. */
};
typedef struct fe_uvec2_coord fe_uvec2_coord;

/*! \brief TODO */
#define fe_uvec2_as_array(v) (v.at)
/*! \brief TODO */
#define fe_uvec2_as_color(v) ((fe_uvec2_color*)fe_uvec2_as_array(v))
/*! \brief TODO */
#define fe_uvec2_as_coord(v) ((fe_uvec2_coord*)fe_uvec2_as_array(v))

/*! \brief TODO */
static inline void fe_uvec2_add(fe_uvec2 * r, const fe_uvec2 * a, const fe_uvec2 * b) {
	size_t i;
	for(i=0; i<2; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_uvec2_sub(fe_uvec2 * r, const fe_uvec2 * a, const fe_uvec2 * b) {
	size_t i;
	for(i=0; i<2; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_uvec2_scale(fe_uvec2 * r, const fe_uvec2 * v, const uint32_t s) {
	size_t i;
	for(i=0; i<2; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_uvec2_dot(a,b) fe_uvec2_mul_inner(a,b)
/*! \brief TODO */
static inline uint32_t fe_uvec2_mul_inner(const fe_uvec2 * a, const fe_uvec2 * b) {
	uint32_t p;
	size_t i;
	for(p=i=0; i<2; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_uvec2_len(v)  sqrt(fe_uvec2_mul_inner(v, v))
/*! \brief TODO */
#define fe_uvec2_lenf(v) sqrtf(fe_uvec2_mul_inner(v, v))
/*! \brief TODO */
#define fe_uvec2_norm(r,v) fe_uvec2_scale(r, v, 1./fe_uvec2_len(v))

/* No cross product for fe_uvec2. */

/*! \brief TODO */
static inline void fe_uvec2_reflect(fe_uvec2 * r, const  fe_uvec2 * v, const fe_uvec2 * n) {
	const uint32_t p = 2*fe_uvec2_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<2 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_UVEC2_H */
