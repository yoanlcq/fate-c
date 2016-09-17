#ifndef FE_GL_SRC
#define FE_GL_SRC

#include <fate/gl.h>
#include <fate/timestamp.h>
#include <fate/iov.h>

fe_timestamp fe_gl_src_get_build_timestamp(void);
typedef struct {
    bool es    : 1;
    bool debug : 1;
    bool optimize : 1;
} fe_gl_src_config;

void fe_gl_src_cube_prelink(const fe_gl_shader_source_set *ss, GLuint prog, fe_timestamp last_build_time);
void fe_gl_src_get_cube_vert(fe_iov *iov, const fe_gl_src_config *cfg);
void fe_gl_src_get_cube_frag(fe_iov *iov, const fe_gl_src_config *cfg);


#endif /* FE_GL_SRC */
