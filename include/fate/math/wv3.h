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

/*! \file fate/math/wv3.h
 *  \brief wv3
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

#ifndef FE_MATH_WV3_H
#define FE_MATH_WV3_H

#ifndef FE_MATH_WITHOUT_VECTOR_EXTENSIONS
    #ifdef __clang__
        #if __has_extension(attribute_ext_vector_type) \
        && __has_builtin(__builtin_shufflevector)
            #include <fate/math/vext/wv3.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
#elif defined(__GNUC__)
        #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
            #include <fate/math/vext/wv3.h>
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
#include <fate/units.h>
/*! \brief TODO 
 *
 * fe_wv3
 */
typedef struct { 
    union {
        fe_space_unit at[3]; 
        struct {
            fe_space_unit r;
            fe_space_unit g;
            fe_space_unit b;
            /* No alpha component. */
        };
        struct {
            fe_space_unit x;
            fe_space_unit y;
            fe_space_unit z;
            /* No w component. */
        };
        struct {
            fe_space_unit s;
            fe_space_unit t;
            fe_space_unit p;
            /* No q component. */
        };
    };
} fe_wv3;

FE_COMPILETIME_ASSERT(offsetof(fe_wv3, r) == offsetof(fe_wv3, at[0]), "");
FE_COMPILETIME_ASSERT(offsetof(fe_wv3, g) == offsetof(fe_wv3, at[1]), "");
FE_COMPILETIME_ASSERT(offsetof(fe_wv3, b) == offsetof(fe_wv3, at[2]), "");


/*! \brief TODO */
static inline void fe_wv3_shuffle(fe_wv3 *r, 
                    const fe_wv3 *v, 
                    fe_space_unit m0, fe_space_unit m1
                    , fe_space_unit m2
                    
                    ) {
    fe_wv3 tmp = *v; /* Aliasing is allowed */
    r->at[0] = tmp.at[m0%3];
    r->at[1] = tmp.at[m1%3];
    r->at[2] = tmp.at[m2%3];
    
}

/*! \brief TODO */
static inline void fe_wv3_shuffle2(fe_wv3 *r, 
                    const fe_wv3 *u,
                    const fe_wv3 *v,
                    fe_space_unit m0, fe_space_unit m1
                    , fe_space_unit m2
                    
                    ) {
    fe_space_unit ary[2*3] = {
        u->at[0], u->at[1]
        , u->at[2]
          
        , v->at[0], v->at[1]
        , v->at[2]
          
    };
    r->at[0] = ary[m0%(2*3)];
    r->at[1] = ary[m1%(2*3)];
    r->at[2] = ary[m2%(2*3)];
    
}


/*! \brief TODO */
static inline void fe_wv3_add(fe_wv3 * r, const fe_wv3 * a, const fe_wv3 * b) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_wv3_sub(fe_wv3 * r, const fe_wv3 * a, const fe_wv3 * b) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_wv3_scale(fe_wv3 * r, const fe_wv3 * v, const fe_space_unit s) {
	size_t i;
	for(i=0; i<3; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_wv3_dot(a,b) fe_wv3_mul_inner(a,b)
/*! \brief TODO */
static inline fe_space_unit fe_wv3_mul_inner(const fe_wv3 * a, const fe_wv3 * b) {
	fe_space_unit p;
	size_t i;
	for(p=i=0; i<3; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_wv3_len(v)  sqrt(fe_wv3_mul_inner(v, v))
/*! \brief TODO */
#define fe_wv3_lenf(v) sqrtf(fe_wv3_mul_inner(v, v))
/*! \brief TODO */
#define fe_wv3_norm(r,v) fe_wv3_scale(r, v, 1./fe_wv3_len(v))

/*! \brief TODO */
#define fe_wv3_cross(r,a,b) fe_wv3_mul_cross(r,a,b)
/*! \brief TODO */
static inline void fe_wv3_mul_cross(fe_wv3 * r, const fe_wv3 * a, const fe_wv3 * b) {
	r->at[0] = a->at[1]*b->at[2] - a->at[2]*b->at[1];
	r->at[1] = a->at[2]*b->at[0] - a->at[0]*b->at[2];
	r->at[2] = a->at[0]*b->at[1] - a->at[1]*b->at[0];
	
}

/*! \brief TODO */
static inline void fe_wv3_reflect(fe_wv3 * r, const  fe_wv3 * v, const fe_wv3 * n) {
	const fe_space_unit p = 2*fe_wv3_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<3 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_WV3_H */
