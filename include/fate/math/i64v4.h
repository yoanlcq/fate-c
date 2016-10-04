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

/*! \file fate/math/i64v4.h
 *  \brief i64v4
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

#ifndef FE_MATH_I64V4_H
#define FE_MATH_I64V4_H

#include <fate/defs.h>
#include <fate/decl.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>



#ifndef FE_MATH_WITHOUT_VECTOR_EXTENSIONS
    #ifdef __clang__
        #if __has_extension(attribute_ext_vector_type) \
        && __has_builtin(__builtin_shufflevector)
            #include <fate/math/vext/i64v4.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
#elif defined(__GNUC__)
        #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
            #include <fate/math/vext/i64v4.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
    #endif
#endif

#ifdef FE_MATH_WITHOUT_VECTOR_EXTENSIONS /* Naive */

/*! \brief TODO 
 *
 * fe_i64v4
 */
typedef struct { 
    union {
        int64_t at[4]; 
        struct {
            int64_t r;
            int64_t g;
            int64_t b;
            int64_t a;
        };
        struct {
            int64_t x;
            int64_t y;
            int64_t z;
            int64_t w;
        };
        struct {
            int64_t s;
            int64_t t;
            int64_t p;
            int64_t q;
        };
    };
} fe_i64v4;

fe_static_assert(offsetof(fe_i64v4, r) == offsetof(fe_i64v4, at[0]), "");
fe_static_assert(offsetof(fe_i64v4, g) == offsetof(fe_i64v4, at[1]), "");
fe_static_assert(offsetof(fe_i64v4, b) == offsetof(fe_i64v4, at[2]), "");
fe_static_assert(offsetof(fe_i64v4, a) == offsetof(fe_i64v4, at[3]), "");

/*! \brief TODO */
static inline void fe_i64v4_shuffle(fe_i64v4 *r, 
                    const fe_i64v4 *v, 
                    uint64_t m0, uint64_t m1
                    , uint64_t m2
                    , uint64_t m3
                    ) {
    fe_i64v4 tmp = *v; /* Aliasing is allowed */
    r->at[0] = tmp.at[m0%4];
    r->at[1] = tmp.at[m1%4];
    r->at[2] = tmp.at[m2%4];
    r->at[3] = tmp.at[m3%4];
}

/*! \brief TODO */
static inline void fe_i64v4_shuffle2(fe_i64v4 *r, 
                    const fe_i64v4 *u,
                    const fe_i64v4 *v,
                    uint64_t m0, uint64_t m1
                    , uint64_t m2
                    , uint64_t m3
                    ) {
    int64_t ary[2*4] = {
        u->at[0], u->at[1]
        , u->at[2]
        , u->at[3]  
        , v->at[0], v->at[1]
        , v->at[2]
        , v->at[3]  
    };
    r->at[0] = ary[m0%(2*4)];
    r->at[1] = ary[m1%(2*4)];
    r->at[2] = ary[m2%(2*4)];
    r->at[3] = ary[m3%(2*4)];
}


/*! \brief TODO */
static inline void fe_i64v4_add(fe_i64v4 * r, const fe_i64v4 * a, const fe_i64v4 * b) {
	size_t i;
	for(i=0; i<4; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_i64v4_sub(fe_i64v4 * r, const fe_i64v4 * a, const fe_i64v4 * b) {
	size_t i;
	for(i=0; i<4; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_i64v4_scale(fe_i64v4 * r, const fe_i64v4 * v, const int64_t s) {
	size_t i;
	for(i=0; i<4; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_i64v4_dot(a,b) fe_i64v4_mul_inner(a,b)
/*! \brief TODO */
static inline int64_t fe_i64v4_mul_inner(const fe_i64v4 * a, const fe_i64v4 * b) {
	int64_t p;
	size_t i;
	for(p=i=0; i<4; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_i64v4_len(v)  sqrt(fe_i64v4_mul_inner(v, v))
/*! \brief TODO */
#define fe_i64v4_lenf(v) sqrtf(fe_i64v4_mul_inner(v, v))
/*! \brief TODO */
#define fe_i64v4_norm(r,v) fe_i64v4_scale(r, v, 1./fe_i64v4_len(v))

/*! \brief TODO */
#define fe_i64v4_cross(r,a,b) fe_i64v4_mul_cross(r,a,b)
/*! \brief TODO */
static inline void fe_i64v4_mul_cross(fe_i64v4 * r, const fe_i64v4 * a, const fe_i64v4 * b) {
	r->at[0] = a->at[1]*b->at[2] - a->at[2]*b->at[1];
	r->at[1] = a->at[2]*b->at[0] - a->at[0]*b->at[2];
	r->at[2] = a->at[0]*b->at[1] - a->at[1]*b->at[0];
	r->at[3] = 1;
}

/*! \brief TODO */
static inline void fe_i64v4_reflect(fe_i64v4 * r, const  fe_i64v4 * v, const fe_i64v4 * n) {
	const int64_t p = 2*fe_i64v4_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<4 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_I64V4_H */
