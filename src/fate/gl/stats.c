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

/*! \file fate/gl/stats.c
 *  \brief Obtaining statistics on the OpenGL rendering pipeline.
 *
 * TODO
 */

#include <string.h>
#include <fate/gl.h>

#ifdef FE_GL_DBG

struct fgl_stats_soa {
    GLenum targets[11];
    GLuint names[11];
    GLint  bits[11];
    GLuint results[11];
};
static struct fgl_stats_soa fgl_stats = {
    {
        GL_VERTICES_SUBMITTED_ARB, 
        GL_PRIMITIVES_SUBMITTED_ARB,
        GL_VERTEX_SHADER_INVOCATIONS_ARB,
        GL_TESS_CONTROL_SHADER_PATCHES_ARB,
        GL_TESS_EVALUATION_SHADER_INVOCATIONS_ARB,
        GL_GEOMETRY_SHADER_INVOCATIONS,
        GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED_ARB,
        GL_FRAGMENT_SHADER_INVOCATIONS_ARB,
        GL_COMPUTE_SHADER_INVOCATIONS_ARB,
        GL_CLIPPING_INPUT_PRIMITIVES_ARB,
        GL_CLIPPING_OUTPUT_PRIMITIVES_ARB
    }, 
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static void fgl_stats_dummy(void) {}

static void fgl_stats_start_recording(void) {
    unsigned i;
    for(i=0 ; i<11 ; ++i)
        glBeginQuery(fgl_stats.targets[i], fgl_stats.names[i]);
}
static void fgl_stats_end_recording(void) {
    unsigned i;
    for(i=0 ; i<11 ; ++i)
        glEndQuery(fgl_stats.targets[i]);
    for(i=0 ; i<11 ; ++i)
        glGetQueryObjectuiv(fgl_stats.names[i], GL_QUERY_RESULT,
                            fgl_stats.results + i);
}

static void fgl_stats_get_stats_dummy(fe_gl_stats *res) {
    memset(res, 0, sizeof(fe_gl_stats));
}
static void fgl_stats_get_stats(fe_gl_stats *res) {
    res->vertices_submitted             = fgl_stats.results[0];
    res->primitives_submitted           = fgl_stats.results[1];
    res->vert_shader_invocations        = fgl_stats.results[2];
    res->tesc_shader_patches            = fgl_stats.results[3];
    res->tese_shader_invocations        = fgl_stats.results[4];
    res->geom_shader_invocations        = fgl_stats.results[5];
    res->geom_shader_primitives_emitted = fgl_stats.results[6];
    res->frag_shader_invocations        = fgl_stats.results[7];
    res->comp_shader_invocations        = fgl_stats.results[8];
    res->clipping_input_primitives      = fgl_stats.results[9];
    res->clipping_output_primitives     = fgl_stats.results[10];
}

void (*fe_gl_stats_start_recording)(void) = fgl_stats_dummy;
void (*fe_gl_stats_end_recording)(void) = fgl_stats_dummy;
void (*fe_gl_stats_get_stats)(fe_gl_stats *s) = fgl_stats_get_stats_dummy;

void fe_gl_stats_setup(void) {
    unsigned i;
    if(GLAD_GL_ARB_pipeline_statistics_query) {
        glGenQueries(11, fgl_stats.names);
        for(i=0 ; i<11 ; ++i)
            glGetQueryiv(fgl_stats.targets[i], GL_QUERY_COUNTER_BITS, 
                         fgl_stats.bits + i);
        fe_gl_stats_start_recording = fgl_stats_start_recording;
        fe_gl_stats_end_recording = fgl_stats_end_recording;
        fe_gl_stats_get_stats = fgl_stats_get_stats;
    } else {
        memset(fgl_stats.names, 0, 11*sizeof(GLuint));
        fe_gl_stats_start_recording = fgl_stats_dummy;
        fe_gl_stats_end_recording = fgl_stats_dummy;
        fe_gl_stats_get_stats = fgl_stats_get_stats_dummy;
    }
}

void fe_gl_stats_cleanup(void) {
    glDeleteQueries(11, fgl_stats.names);
}

#endif
