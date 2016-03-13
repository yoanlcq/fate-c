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

/*! \file fate/math/ivec3.h
 *  \brief ivec3
 *  \ingroup math
 * 
 * TODO
 *
 * @{
 */

#ifndef FATE_MATH_IVEC3_H
#define FATE_MATH_IVEC3_H


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
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO 
 *
 * ivec3
 */
typedef int32_t ivec3[3];

/*! \brief TODO */
struct FATE_PACKED_STRUCT ivec3_color {
    int32_t r;
    int32_t g;
    int32_t b;
    /* No alpha component. */
};
typedef struct ivec3_color ivec3_color;

/*! \brief TODO */
struct FATE_PACKED_STRUCT ivec3_coord {
    int32_t x;
    int32_t y;
    int32_t z;
    /* No w component. */
};
typedef struct ivec3_coord ivec3_coord;

/*! \brief TODO */
#define ivec3_as_array(v) (v)
/*! \brief TODO */
#define ivec3_as_color(v) ((ivec3_color*)ivec3_as_array(v))
/*! \brief TODO */
#define ivec3_as_coord(v) ((ivec3_coord*)ivec3_as_array(v))

/*! \brief TODO */
static inline void ivec3_add(ivec3 r, const ivec3 a, const ivec3 b) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = a[i] + b[i];
}
/*! \brief TODO */
static inline void ivec3_sub(ivec3 r, const ivec3 a, const ivec3 b) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = a[i] - b[i];
}
/*! \brief TODO */
static inline void ivec3_scale(ivec3 r, const ivec3 v, const int32_t s) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = v[i] * s;
}

/*! \brief TODO */
#define ivec3_dot(a,b) ivec3_mul_inner(a,b)
/*! \brief TODO */
static inline int32_t ivec3_mul_inner(const ivec3 a, const ivec3 b) {
	int32_t p;
	size_t i;
	for(p=i=0; i<3; ++i)
		p += b[i]*a[i];
	return p;
}

/*! \brief TODO */
#define ivec3_len(v)  sqrt(ivec3_mul_inner(v, v))
/*! \brief TODO */
#define ivec3_lenf(v) sqrtf(ivec3_mul_inner(v, v))
/*! \brief TODO */
#define ivec3_norm(r,v) ivec3_scale(r, v, 1./ivec3_len(v))

/*! \brief TODO */
#define ivec3_cross(r,a,b) ivec3_mul_cross(r,a,b)
/*! \brief TODO */
static inline void ivec3_mul_cross(ivec3 r, const ivec3 a, const ivec3 b) {
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
	
}

/*! \brief TODO */
static inline void ivec3_reflect(ivec3 r, const ivec3 v, const ivec3 n) {
	const int32_t p = 2*ivec3_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<3 ; ++i)
		r[i] = v[i] - p*n[i];
}

#endif /* Naive */

#endif /* FATE_MATH_IVEC3_H */

/* @} */
