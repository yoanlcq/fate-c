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

/*! \file fate/math/vec3.h
 *  \brief vec3
 *  \ingroup math
 * 
 * TODO
 *
 * @{
 */

#ifndef FATE_MATH_VEC3_H
#define FATE_MATH_VEC3_H


#ifdef __clang__
    #if __has_extension(attribute_ext_vector_type) \
    && __has_builtin(__builtin_shufflevector)
        #include <fate/math/vext/vec3.h>
    #endif
#elif defined(__GNUC__)
    #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
        #include <fate/math/vext/vec3.h>
    #endif

#else /* Naive */

#include <fate/defs.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO 
 *
 * vec3
 */
typedef float vec3[3];

/*! \brief TODO */
struct FATE_PACKED_STRUCT vec3_color {
    float r;
    float g;
    float b;
    /* No alpha component. */
};
typedef struct vec3_color vec3_color;

/*! \brief TODO */
struct FATE_PACKED_STRUCT vec3_coord {
    float x;
    float y;
    float z;
    /* No w component. */
};
typedef struct vec3_coord vec3_coord;

/*! \brief TODO */
#define vec3_as_array(v) (v)
/*! \brief TODO */
#define vec3_as_color(v) ((vec3_color*)vec3_as_array(v))
/*! \brief TODO */
#define vec3_as_coord(v) ((vec3_coord*)vec3_as_array(v))

/*! \brief TODO */
static inline void vec3_add(vec3 r, const vec3 a, const vec3 b) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = a[i] + b[i];
}
/*! \brief TODO */
static inline void vec3_sub(vec3 r, const vec3 a, const vec3 b) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = a[i] - b[i];
}
/*! \brief TODO */
static inline void vec3_scale(vec3 r, const vec3 v, const float s) {
	size_t i;
	for(i=0; i<3; ++i)
		r[i] = v[i] * s;
}

/*! \brief TODO */
#define vec3_dot(a,b) vec3_mul_inner(a,b)
/*! \brief TODO */
static inline float vec3_mul_inner(const vec3 a, const vec3 b) {
	float p;
	size_t i;
	for(p=i=0; i<3; ++i)
		p += b[i]*a[i];
	return p;
}

/*! \brief TODO */
#define vec3_len(v)  sqrt(vec3_mul_inner(v, v))
/*! \brief TODO */
#define vec3_lenf(v) sqrtf(vec3_mul_inner(v, v))
/*! \brief TODO */
#define vec3_norm(r,v) vec3_scale(r, v, 1./vec3_len(v))

/*! \brief TODO */
#define vec3_cross(r,a,b) vec3_mul_cross(r,a,b)
/*! \brief TODO */
static inline void vec3_mul_cross(vec3 r, const vec3 a, const vec3 b) {
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
	
}

/*! \brief TODO */
static inline void vec3_reflect(vec3 r, const vec3 v, const vec3 n) {
	const float p = 2*vec3_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<3 ; ++i)
		r[i] = v[i] - p*n[i];
}

#endif /* Naive */

#endif /* FATE_MATH_VEC3_H */

/* @} */
