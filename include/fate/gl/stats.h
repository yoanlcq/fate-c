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

/*! \file fate/gl/stats.h
 *  \brief OpenGL wrapper for getting rendering pipeline statistics.
 *
 *  TODO
 */

#ifndef FE_GL_STATS_H
#define FE_GL_STATS_H

#include <fate/gl/defs.h>
#include <fate/gl/debug.h>

struct fe_gl_stats {
    GLuint vertices_submitted;
    GLuint primitives_submitted;
    GLuint vert_shader_invocations;
    GLuint tesc_shader_patches;
    GLuint tese_shader_invocations;
    GLuint geom_shader_invocations;
    GLuint geom_shader_primitives_emitted;
    GLuint frag_shader_invocations;
    GLuint comp_shader_invocations;
    GLuint clipping_input_primitives;
    GLuint clipping_output_primitives;
};
typedef struct fe_gl_stats fe_gl_stats;

#ifdef FE_GL_DEBUG
void fe_gl_stats_setup(void);
void fe_gl_stats_cleanup(void);
extern void (*fe_gl_stats_start_recording)(void);
extern void (*fe_gl_stats_end_recording)(void);
extern void (*fe_gl_stats_get_stats)(fe_gl_stats*);
#else
#define fe_gl_stats_setup() 
#define fe_gl_stats_cleanup() 
#define fe_gl_stats_start_recording() 
#define fe_gl_stats_end_recording() 
#define fe_gl_stats_get_stats(X) 
#endif /* FE_GL_DEBUG */

#endif /* FE_GL_STATS_H */
