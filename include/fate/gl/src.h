#ifndef FE_GL_SRC
#define FE_GL_SRC

#include <fate/gl.h>
#include <fate/timestamp.h>

fe_timestamp fe_gl_src_get_build_timestamp(void);
void fe_gl_src_before_linking(const fe_gl_shader_source_set *ss, GLuint prog, fe_timestamp last_build_time);
extern const char *const fe_gl_src_tri_130_vert;
extern const char *const fe_gl_src_tri_130_frag;

#endif /* FE_GL_SRC */
