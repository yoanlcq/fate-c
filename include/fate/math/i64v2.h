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

/*! \file fate/math/i64v2.h
 *  \brief i64v2
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

#ifndef FE_MATH_I64V2_H
#define FE_MATH_I64V2_H

#ifndef FE_MATH_WITHOUT_VECTOR_EXTENSIONS
    #ifdef __clang__
        #if __has_extension(attribute_ext_vector_type) \
        && __has_builtin(__builtin_shufflevector)
            #include <fate/math/vext/i64v2.h>
        #else
            #define FE_MATH_WITHOUT_VECTOR_EXTENSIONS
        #endif
#elif defined(__GNUC__)
        #if __GNUC__>4 || (__GNUC__==4 && __GNUC_MINOR__>=7)
            #include <fate/math/vext/i64v2.h>
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
 * fe_i64v2
 */
typedef struct { 
    union {
        int64_t at[2]; 
        struct {
            int64_t r;
            int64_t g;
            /* No blue component. */
            /* No alpha component. */
        };
        struct {
            int64_t x;
            int64_t y;
            /* No z component. */
            /* No w component. */
        };
        struct {
            int64_t s;
            int64_t t;
            /* No p component. */
            /* No q component. */
        };
    };
} fe_i64v2;

FE_COMPILETIME_ASSERT(offsetof(fe_i64v2, r) == offsetof(fe_i64v2, at[0]), "");
FE_COMPILETIME_ASSERT(offsetof(fe_i64v2, g) == offsetof(fe_i64v2, at[1]), "");



/*! \brief TODO */
static inline void fe_i64v2_shuffle(fe_i64v2 *r, 
                    const fe_i64v2 *v, 
                    uint64_t m0, uint64_t m1
                    
                    
                    ) {
    fe_i64v2 tmp = *v; /* Aliasing is allowed */
    r->at[0] = tmp.at[m0%2];
    r->at[1] = tmp.at[m1%2];
    
    
}

/*! \brief TODO */
static inline void fe_i64v2_shuffle2(fe_i64v2 *r, 
                    const fe_i64v2 *u,
                    const fe_i64v2 *v,
                    uint64_t m0, uint64_t m1
                    
                    
                    ) {
    int64_t ary[2*2] = {
        u->at[0], u->at[1]
        
          
        , v->at[0], v->at[1]
        
          
    };
    r->at[0] = ary[m0%(2*2)];
    r->at[1] = ary[m1%(2*2)];
    
    
}


/*! \brief TODO */
static inline void fe_i64v2_add(fe_i64v2 * r, const fe_i64v2 * a, const fe_i64v2 * b) {
	size_t i;
	for(i=0; i<2; ++i)
		r->at[i] = a->at[i] + b->at[i];
}
/*! \brief TODO */
static inline void fe_i64v2_sub(fe_i64v2 * r, const fe_i64v2 * a, const fe_i64v2 * b) {
	size_t i;
	for(i=0; i<2; ++i)
		r->at[i] = a->at[i] - b->at[i];
}
/*! \brief TODO */
static inline void fe_i64v2_scale(fe_i64v2 * r, const fe_i64v2 * v, const int64_t s) {
	size_t i;
	for(i=0; i<2; ++i)
		r->at[i] = v->at[i] * s;
}

/*! \brief TODO */
#define fe_i64v2_dot(a,b) fe_i64v2_mul_inner(a,b)
/*! \brief TODO */
static inline int64_t fe_i64v2_mul_inner(const fe_i64v2 * a, const fe_i64v2 * b) {
	int64_t p;
	size_t i;
	for(p=i=0; i<2; ++i)
		p += b->at[i]*a->at[i];
	return p;
}

/*! \brief TODO */
#define fe_i64v2_len(v)  sqrt(fe_i64v2_mul_inner(v, v))
/*! \brief TODO */
#define fe_i64v2_lenf(v) sqrtf(fe_i64v2_mul_inner(v, v))
/*! \brief TODO */
#define fe_i64v2_norm(r,v) fe_i64v2_scale(r, v, 1./fe_i64v2_len(v))

/* No cross product for fe_i64v2. */

/*! \brief TODO */
static inline void fe_i64v2_reflect(fe_i64v2 * r, const  fe_i64v2 * v, const fe_i64v2 * n) {
	const int64_t p = 2*fe_i64v2_mul_inner(v, n);
	size_t i;
	for(i=0 ; i<2 ; ++i)
		r->at[i] = v->at[i] - p*n->at[i];
}

#endif /* Naive */

/* @} */
#endif /* FE_MATH_I64V2_H */
