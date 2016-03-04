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

/*! \file fate/math/spacevec3.h
 *  \brief spacevec3
 *  \ingroup math
 * 
 * TODO
 *
 * @{
 */

#ifndef FATE_MATH_SPACEVEC3_H
#define FATE_MATH_SPACEVEC3_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/spacevec3.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/spacevec3.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <fate/units.h>
/*! \brief TODO */
typedef fate_space_unit spacevec3[3];

/*! \brief TODO */
struct FATE_PACKED_STRUCT spacevec3_color {
    fate_space_unit r;
    fate_space_unit g;
    fate_space_unit b;
    /* No alpha component. */
};
typedef struct spacevec3_color spacevec3_color;

/*! \brief TODO */
struct FATE_PACKED_STRUCT spacevec3_coord {
    fate_space_unit x;
    fate_space_unit y;
    fate_space_unit z;
    /* No w component. */
};
typedef struct spacevec3_coord spacevec3_coord;

/*! \brief TODO */
#define spacevec3_as_array(v) (v)
/*! \brief TODO */
#define spacevec3_as_color(v) ((spacevec3_color*)spacevec3_as_array(v))
/*! \brief TODO */
#define spacevec3_as_coord(v) ((spacevec3_coord*)spacevec3_as_array(v))

/*! \brief TODO */
static inline void spacevec3_add(spacevec3 r, const spacevec3 a, const spacevec3 b) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = a[i] + b[i];
}
/*! \brief TODO */
static inline void spacevec3_sub(spacevec3 r, const spacevec3 a, const spacevec3 b) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = a[i] - b[i];
}
/*! \brief TODO */
static inline void spacevec3_scale(spacevec3 r, const spacevec3 v, const fate_space_unit s) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = v[i] * s;
}

/*! \brief TODO */
#define spacevec3_dot(a,b) spacevec3_mul_inner(a,b)
/*! \brief TODO */
static inline fate_space_unit spacevec3_mul_inner(const spacevec3 a, const spacevec3 b) {
	fate_space_unit p;
	size_t i;
	for(p=i=0; i<3; ++i)
		p += b[i]*a[i];
	return p;
}

/*! \brief TODO */
#define spacevec3_len(v)  sqrt(spacevec3_mul_inner(v, v))
/*! \brief TODO */
#define spacevec3_lenf(v) sqrtf(spacevec3_mul_inner(v, v))
/*! \brief TODO */
#define spacevec3_norm(r,v) spacevec3_scale(r, v, 1./spacevec3_len(v))

/*! \brief TODO */
#define spacevec3_cross(r,a,b) spacevec3_mul_cross(r,a,b)
/*! \brief TODO */
static inline void spacevec3_mul_cross(spacevec3 r, const spacevec3 a, const spacevec3 b) {
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
	
}

/*! \brief TODO */
static inline void spacevec3_reflect(spacevec3 r, const spacevec3 v, const spacevec3 n) {
	const fate_space_unit p = 2*spacevec3_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<3 ; ++i)
		r[i] = v[i] - p*n[i];
}

#endif /* Naive */

#endif /* FATE_MATH_SPACEVEC3_H */

/* @} */
