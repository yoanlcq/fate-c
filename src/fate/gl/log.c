#include <stdlib.h>
#include <stdbool.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/gl/defs.h>
#include <fate/gl/log.h>

void fate_gl_log_error(GLenum err, bool even_no_error) {
#define HELPER(C,X) case X: C fate_logf_video("%d (%s)", X, #X); break
    switch(err) {
    HELPER(,GL_INVALID_ENUM);
    HELPER(,GL_INVALID_VALUE);
    HELPER(,GL_INVALID_OPERATION);
    HELPER(,GL_INVALID_FRAMEBUFFER_OPERATION);
    HELPER(,GL_OUT_OF_MEMORY);
    HELPER(,GL_STACK_UNDERFLOW);
    HELPER(,GL_STACK_OVERFLOW);
    HELPER(if(even_no_error),GL_NO_ERROR);
    }
#undef HELPER
}

void fate_gl_log_all_errors(void) {
    GLenum err;
    for(err = glGetError() ; err != GL_NO_ERROR ; err = glGetError()) {
        fate_logf_video("glGetError() returned ");
        fate_gl_log_error(err, false);
        fate_logf_video("\n");
    }
}

void fate_gl_log_shader_info(GLuint shader) {
    GLchar *err;
    GLint errlen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errlen);
    err = malloc((errlen+1)*sizeof(GLchar));
    glGetShaderInfoLog(shader, errlen, &errlen, err);
    err[errlen] = '\0';
    fate_logf_video(err);
    free(err);
}

void fate_gl_log_program_info(GLuint program) {
    GLchar *err;
    GLsizei errlen;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errlen);
    err = malloc((errlen+1)*sizeof(GLchar));
    glGetProgramInfoLog(program, errlen, &errlen, err);
    err[errlen] = '\0';
    fate_logf_video(err);
    free(err);
}
