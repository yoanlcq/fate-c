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

/*! \file fate/videobackend.h
 *  \brief Video backends.
 *  \defgroup videobackend Video backends. TODO
 *
 * TODO
 *
 * @{
 */


#ifndef FE_VIDEOBACKEND_H
#define FE_VIDEOBACKEND_H

/*! \brief TODO*/
enum fe_videobackend_api {
    FE_VIDEOBACKEND_DUMMY     = 0,
    FE_VIDEOBACKEND_OPENGL    = 1,
    FE_VIDEOBACKEND_OPENGLES  = 2,
    FE_VIDEOBACKEND_VULKAN    = 3,
    FE_VIDEOBACKEND_DIRECT3D  = 4,
    FE_VIDEOBACKEND_MANTLE    = 5,
    FE_VIDEOBACKEND_METAL     = 6,
    FE_VIDEOBACKEND_GLIDE     = 7,
    FE_VIDEOBACKEND_RENDERMAN = 8
};
typedef enum fe_videobackend_api fe_videobackend_api;

/*! \brief TODO*/
struct fe_gl_videobackend {
    fe_videobackend_api api;
    SDL_GL_context ctx;
    unsigned software      : 1; //Use Mesa ?
    unsigned major_version : 4;
    unsigned minor_version : 4;
    unsigned context_flags :10; //debug, forward-compatible
    unsigned profile_flags : 3; //core/compatibility;
    unsigned double_buffer : 1;
    unsigned stereo_buffer : 1;
    uint8_t  aux_buffers;
    uint8_t  red_bits;
    uint8_t  green_bits;
    uint8_t  blue_bits;
    uint8_t  alpha_bits;
    uint8_t  depth_bits;
    uint8_t  stencil_bits;
    uint8_t  accum_red_bits;
    uint8_t  accum_green_bits;
    uint8_t  accum_blue_bits;
    uint8_t  accum_alpha_bits;
};
typedef struct fe_gl_videobackend fe_gl_videobackend;


/*! \brief TODO*/
FE_DECL_NIY void fe_videobackend_get_default(fe_videobackend *out);
/*! \brief TODO
 * 
 * When it fails or is NIY, the obtained backend is Dummy. */
FE_DECL_NIY void fe_videobackend_create(const fe_videobackend *requested,             
                                    fe_videobackend *obtained);
/*! \brief TODO*/
FE_DECL_NIY void fe_videobackend_delete(fe_videobackend *videobackend);
/*! \brief TODO*/
FE_DECL_NIY void fe_videobackend_use(fe_videobackend *videobackend);
/*! \brief TODO*/
FE_DECL_NIY void fe_videobackend_copy(fe_videobackend *to,
                            const fe_videobackend *from);


#endif  /* FE_VIDEOBACKEND_H */

/* @} */
