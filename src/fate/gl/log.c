#include <stdlib.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/gl/defs.h>
#include <fate/gl/log.h>

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
