#include <fate/gl/defs.h>
#include <stdbool.h>

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

void fate_gl_log_all_errors() {
    GLenum err;
    for(err = glGetError() ; err != GL_NO_ERROR ; err = glGetError()) {
        fate_logf_video("glGetError() returned ");
        fate_gl_log_error(err, false);
        fate_logf_video("\n");
    }
}

#ifdef GL_CHECK_ERRORS
#define GL_CHECK_ERR() print_gl_errors(stdout)
#else
#define GL_CHECK_ERR()
#endif

