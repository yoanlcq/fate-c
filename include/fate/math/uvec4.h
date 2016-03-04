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

/*! \file fate/math/uvec4.h
 *  \brief uvec4
 *  \ingroup math
 * 
 * TODO
 *
 * @{
 */

#ifndef FATE_MATH_UVEC4_H
#define FATE_MATH_UVEC4_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/uvec4.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/uvec4.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO */
typedef uint32_t uvec4[4];

/*! \brief TODO */
struct FATE_PACKED_STRUCT uvec4_color {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
};
typedef struct uvec4_color uvec4_color;

/*! \brief TODO */
struct FATE_PACKED_STRUCT uvec4_coord {
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t w;
};
typedef struct uvec4_coord uvec4_coord;

/*! \brief TODO */
#define uvec4_as_array(v) (v)
/*! \brief TODO */
#define uvec4_as_color(v) ((uvec4_color*)uvec4_as_array(v))
/*! \brief TODO */
#define uvec4_as_coord(v) ((uvec4_coord*)uvec4_as_array(v))

/*! \brief TODO */
static inline void uvec4_add(uvec4 r, const uvec4 a, const uvec4 b) {
	size_t i;
	for(i=0; i<4; ++i)
		r[i] = a[i] + b[i];
}
/*! \brief TODO */
static inline void uvec4_sub(uvec4 r, const uvec4 a, const uvec4 b) {
	size_t i;
	for(i=0; i<4; ++i)
		r[i] = a[i] - b[i];
}
/*! \brief TODO */
static inline void uvec4_scale(uvec4 r, const uvec4 v, const uint32_t s) {
	size_t i;
	for(i=0; i<4; ++i)
		r[i] = v[i] * s;
}

/*! \brief TODO */
#define uvec4_dot(a,b) uvec4_mul_inner(a,b)
/*! \brief TODO */
static inline uint32_t uvec4_mul_inner(const uvec4 a, const uvec4 b) {
	uint32_t p;
	size_t i;
	for(p=i=0; i<4; ++i)
		p += b[i]*a[i];
	return p;
}

/*! \brief TODO */
#define uvec4_len(v)  sqrt(uvec4_mul_inner(v, v))
/*! \brief TODO */
#define uvec4_lenf(v) sqrtf(uvec4_mul_inner(v, v))
/*! \brief TODO */
#define uvec4_norm(r,v) uvec4_scale(r, v, 1./uvec4_len(v))

/*! \brief TODO */
#define uvec4_cross(r,a,b) uvec4_mul_cross(r,a,b)
/*! \brief TODO */
static inline void uvec4_mul_cross(uvec4 r, const uvec4 a, const uvec4 b) {
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
	r[3] = 1.f;
}

/*! \brief TODO */
static inline void uvec4_reflect(uvec4 r, const uvec4 v, const uvec4 n) {
	const uint32_t p = 2*uvec4_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<4 ; ++i)
		r[i] = v[i] - p*n[i];
}

#endif /* Naive */

#endif /* FATE_MATH_UVEC4_H */

/* @} */
