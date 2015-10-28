#ifndef OPENGL_BASE_H
#define OPENGL_BASE_H

#define GL_GLEXT_PROTOTYPES
#define GLEW_STATIC
#include <GL/glew.h>
#include <SFML/OpenGL.h>
#include <GL/glext.h>
#ifndef GL_VERSION_4_3
#error We target OpenGL 4.3.
#endif
#define BUFFER_OFFSET(_i_) ((void*)_i_)

#endif /* OPENGL_BASE_H */
