
/*
void print_gl_errors(FILE *stream) {
    GLenum err;
    const char *str;
    for(err = glGetError() ; err != GL_NO_ERROR ; err = glGetError())
    {
#define HELPER(X) case X: str = #X; break
        switch(err) {
        HELPER(GL_INVALID_ENUM);
        HELPER(GL_INVALID_VALUE);
        HELPER(GL_INVALID_OPERATION);
        HELPER(GL_INVALID_FRAMEBUFFER_OPERATION);
        HELPER(GL_OUT_OF_MEMORY);
        HELPER(GL_STACK_UNDERFLOW);
        HELPER(GL_STACK_OVERFLOW);
        }
#undef HELPER
        fprintf(stream, "glGetError() returned %x (%s)\n", err, str);
    }
}

#ifdef GL_CHECK_ERRORS
#define GL_CHECK_ERR() print_gl_errors(stdout)
#else
#define GL_CHECK_ERR()
#endif
*/


