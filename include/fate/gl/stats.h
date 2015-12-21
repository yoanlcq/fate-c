#ifndef FATE_GL_STATS_H
#define FATE_GL_STATS_H

#include <fate/gl/defs.h>
#include <fate/gl/debug.h>

struct fate_gl_stats {
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
typedef struct fate_gl_stats fate_gl_stats;

#ifdef FATE_GL_DEBUG
void fate_gl_stats_setup(void);
void fate_gl_stats_cleanup(void);
extern void (*fate_gl_stats_start_recording)(void);
extern void (*fate_gl_stats_end_recording)(void);
extern void (*fate_gl_stats_get_stats)(fate_gl_stats*);
#else
#define fate_gl_stats_setup() 
#define fate_gl_stats_cleanup() 
#define fate_gl_stats_start_recording() 
#define fate_gl_stats_end_recording() 
#define fate_gl_stats_get_stats(X) 
#endif /* FATE_GL_DEBUG */

#endif /* FATE_GL_STATS_H */
