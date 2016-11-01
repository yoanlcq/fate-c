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

/*! \file fate/3d/light.h
 *  \brief 3D lights draft.
 *  \defgroup 3d_light 3D lights draft.
 *
 * @{
 */

#ifndef FE_3D_LIGHT_H
#define FE_3D_LIGHT_H

typedef struct {
    vec3 color;
    vec3 ambient_contrib;
} fe_3d_lightprops;

typedef struct {
    GLint constant_attenuation; // local light attenuation coefficients
    GLint linear_attenuation;
    GLint quadratic_attenuation;
} fe_3d_locallightprops;

typedef struct {
    fe_3d_lightprops
    fe_3d_locallightprops;
    vec3 position;
} fe_3d_pointlight;

typedef struct {
    fe_3d_lightprops
    fe_3d_locallightprops;
    vec3 position;
    vec3 cone_directon;
    float cos_cutoff;
    float exponent;
} fe_3d_spotlight;

typedef struct {
    fe_3d_lightprops
    vec3 direction;
    vec3 half_vector;
} fe_3d_directionallight;

typedef struct {
    vec3 center;
    float radius;
    vec3 direction;
    float percent_used;
} fe_3d_domelight;


/*! @} */

#endif /* FE_3D_LIGHT_H */
