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

/*! \file fate/gl/log.c
 *  \brief OpenGL state loggins utilities.
 *
 * TODO
 */

#include <stdlib.h>
#include <stdbool.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/gl/defs.h>
#include <fate/gl/log.h>

void fe_gl_log_error(GLenum err, bool even_no_error) {
#define HELPER(C,X) case X: C fe_logf_video("%d (%s)", X, #X); break
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

void fe_gl_log_all_errors(void) {
    GLenum err;
    for(err = glGetError() ; err != GL_NO_ERROR ; err = glGetError()) {
        fe_logf_video("glGetError() returned ");
        fe_gl_log_error(err, false);
        fe_logf_video("\n");
    }
}

void fe_gl_log_shader_info(GLuint shader) {
    GLchar *err;
    GLint errlen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errlen);
    err = malloc((errlen+1)*sizeof(GLchar));
    glGetShaderInfoLog(shader, errlen, &errlen, err);
    err[errlen] = '\0';
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    fe_logf_video(err);
#pragma GCC diagnostic pop
    free(err);
}

void fe_gl_log_program_info(GLuint program) {
    GLchar *err;
    GLsizei errlen;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errlen);
    err = malloc((errlen+1)*sizeof(GLchar));
    glGetProgramInfoLog(program, errlen, &errlen, err);
    err[errlen] = '\0';
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    fe_logf_video(err);
#pragma GCC diagnostic pop
    free(err);
}
