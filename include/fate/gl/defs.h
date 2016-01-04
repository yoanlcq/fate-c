#ifndef FATE_GL_DEFS
#define FATE_GL_DEFS

#include <fate/defs.h>

#define GL_GLEXT_PROTOTYPES
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#if defined(FATE_OSX)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
/* #include <GL/glext.h> */
#define BUFFER_OFFSET(_i_) ((void*)_i_)

#endif /* FATE_GL_DEFS */
