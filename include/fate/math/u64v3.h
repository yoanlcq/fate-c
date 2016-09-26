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

/*! \file fate/math/u64v3.h
 *  \brief u64v3
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

#ifndef FE_MATH_U64V3_H
#define FE_MATH_U64V3_H

#ifndef FE_MATH_WITHOUT_VECTOR_EXTENSIONS
    #ifdef __clang__
        #if __has_extension(attribute_ext_vector_type) \
        && __has_builtin(__builtin_shufflevector)
            #include <fate/math/vext/u64v3.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
#elif defined(__GNUC__)
        #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
            #include <fate/math/vext/u64v3.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
    #endif
#endif

#ifdef FE_MATH_WITHOUT_VECTOR_EXTENSIONS /* Naive */

#include <fate/defs.h>
#include <fate/decl.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>

/*! \brief TODO 
 *
 * fe_u64v3
 */
typedef struct { 
    union {
        uint64_t at[3]; 
        struct {
            uint64_t r;
            uint64_t g;
            uint64_t b;
            /* No alpha component. */
        };
        struct {
            uint64_t x;
            uint64_t y;
            uint64_t z;
            /* No w component. */
        };
        struct {
            uint64_t s;
            uint64_t t;
            uint64_t p;
            /* No q component. */
        };
    };
} fe_u64v3;

FE_COMPILETIME_ASSERT(offsetof(fe_u64v3, r) == offsetof(fe_u64v3, at[0]), "");
FE_COMPILETIME_ASSERT(offsetof(fe_u64v3, g) == offsetof(fe_u64v3, at[1]), "");
FE_COMPILETIME_ASSERT(offsetof(fe_u64v3, b) == offsetof(fe_u64v3, at[2]), "");


/*! \brief TODO */
static inline void fe_u64v3_add(fe_u64v3 * r, const fe_u64v3 * a, const fe_u64v3 * b) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_u64v3_sub(fe_u64v3 * r, const fe_u64v3 * a, const fe_u64v3 * b) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_u64v3_scale(fe_u64v3 * r, const fe_u64v3 * v, const uint64_t s) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_u64v3_dot(a,b) fe_u64v3_mul_inner(a,b)
/*! \brief TODO */
static inline uint64_t fe_u64v3_mul_inner(const fe_u64v3 * a, const fe_u64v3 * b) {
	uint64_t p;
	size_t i;
	for(p=i=0; i<3; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_u64v3_len(v)  sqrt(fe_u64v3_mul_inner(v, v))
/*! \brief TODO */
#define fe_u64v3_lenf(v) sqrtf(fe_u64v3_mul_inner(v, v))
/*! \brief TODO */
#define fe_u64v3_norm(r,v) fe_u64v3_scale(r, v, 1./fe_u64v3_len(v))

/*! \brief TODO */
#define fe_u64v3_cross(r,a,b) fe_u64v3_mul_cross(r,a,b)
/*! \brief TODO */
static inline void fe_u64v3_mul_cross(fe_u64v3 * r, const fe_u64v3 * a, const fe_u64v3 * b) {
	r->at[0] = a->at[1]*b->at[2] - a->at[2]*b->at[1];
	r->at[1] = a->at[2]*b->at[0] - a->at[0]*b->at[2];
	r->at[2] = a->at[0]*b->at[1] - a->at[1]*b->at[0];
	
}

/*! \brief TODO */
static inline void fe_u64v3_reflect(fe_u64v3 * r, const  fe_u64v3 * v, const fe_u64v3 * n) {
	const uint64_t p = 2*fe_u64v3_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<3 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_U64V3_H */
