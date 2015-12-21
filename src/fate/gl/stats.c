#include <string.h>
#include <fate/gl/defs.h>
#include <fate/gl/debug.h>
#include <fate/gl/stats.h>

#ifdef FATE_GL_DEBUG

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

static void fgl_stats_get_stats_dummy(fate_gl_stats *res) {
    memset(res, 0, sizeof(fate_gl_stats));
}
static void fgl_stats_get_stats(fate_gl_stats *res) {
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

void (*fate_gl_stats_start_recording)(void) = fgl_stats_dummy;
void (*fate_gl_stats_end_recording)(void) = fgl_stats_dummy;
void (*fate_gl_stats_get_stats)(fate_gl_stats *s) = fgl_stats_get_stats_dummy;

void fate_gl_stats_setup(void) {
    unsigned i;
    if(GLEW_ARB_pipeline_statistics_query) {
        glGenQueries(11, fgl_stats.names);
        for(i=0 ; i<11 ; ++i)
            glGetQueryiv(fgl_stats.targets[i], GL_QUERY_COUNTER_BITS, 
                         fgl_stats.bits + i);
        fate_gl_stats_start_recording = fgl_stats_start_recording;
        fate_gl_stats_end_recording = fgl_stats_end_recording;
        fate_gl_stats_get_stats = fgl_stats_get_stats;
    } else {
        memset(fgl_stats.names, 0, 11*sizeof(GLuint));
        fate_gl_stats_start_recording = fgl_stats_dummy;
        fate_gl_stats_end_recording = fgl_stats_dummy;
        fate_gl_stats_get_stats = fgl_stats_get_stats_dummy;
    }
}

void fate_gl_stats_cleanup(void) {
    glDeleteQueries(11, fgl_stats.names);
}

#endif
