#include <stdbool.h>
#include <string.h>
#include <fate/gl/defs.h>
#include <fate/gl/utils.h>

bool fate_gl_has_extension(const char *ext) {
    GLint i, n;    
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for(i=0 ; i<n ; ++i)
        if(!strcmp(ext, (const char*)glGetStringi(GL_EXTENSIONS, i)))
            return true;
    return false;
}
