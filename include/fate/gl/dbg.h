/**
 * @file opengl_debug.h
 * @author Yoan Lecoq
 * @date 13 Jul 2015
 * @brief Header file for all things "OpenGL debug".
 *
 */

#ifndef FATE_GL_DBG_H
#define FATE_GL_DBG_H

#include <fate/gl.h>

#ifdef FATE_DEFS_DEBUG_BUILD

#ifndef APIENTRY /* Noticeable on Linux */
#define APIENTRY
#endif

typedef void (APIENTRY *DEBUGPROC)  (GLenum source, 
                                     GLenum type, 
                                     GLuint id,
                                     GLenum severity, 
                                     GLsizei length, 
                                     const GLchar* message, 
                                     void* userParam);
void APIENTRY fate_gl_dbg_callback  (GLenum source, 
                                     GLenum type, 
                                     GLuint id,
                                     GLenum severity, 
                                     GLsizei length, 
                                     const GLchar* message, 
                                     void* userParam);

#endif /* FATE_DEFS_DEBUG_BUILD */
#endif /* FATE_GL_DBG_H */
