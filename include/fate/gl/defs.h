#ifndef FATE_GL_DEFS
#define FATE_GL_DEFS

#include <fate/defs.h>

#define GL_GLEXT_PROTOTYPES
#define GLEW_STATIC
#include <GL/glew.h>
#if defined(FATE_DEFS_OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <GL/glext.h>
#define BUFFER_OFFSET(_i_) ((void*)_i_)

#endif /* FATE_GL_DEFS */
