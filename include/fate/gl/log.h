#ifndef FATE_GL_LOG
#define FATE_GL_LOG

#include <stdbool.h>
#include <fate/gl/defs.h>

#ifdef GL_CHECK_ERRORS
#define GL_CHECK_ERR() print_gl_errors(stdout)
#else
#define GL_CHECK_ERR()
#endif

void fate_gl_log_error(GLenum err, bool even_no_error);
void fate_gl_log_all_errors(void);
void fate_gl_log_program_info(GLuint program);
void fate_gl_log_shader_info(GLuint shader);

#endif /* FATE_GL_LOG */
