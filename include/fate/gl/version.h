#ifndef FE_GL_VERSION_H
#define FE_GL_VERSION_H

#include <fate/gl/defs.h>
#include <fate/gl/version.h>
#include <stdbool.h>

typedef struct {
    GLint major, minor;
    bool core :1;
    bool compat :1;
    bool es :1;
} fe_gl_version;

bool fe_gl_version_query(fe_gl_version *v);

#endif /* FE_GL_VERSION_H */
