/**
 * @file opengl_debug.h
 * @author Yoan Lecoq
 * @date 13 Jul 2015
 * @brief Header file to toggle OpenGL debugging statically.
 *
 * This file defines an OpenGL debug callback function, named opengl_debug_callback, and a 
 * macro named OPENGL_DEBUG indicating whether OpenGL debugging is enabled or not, that should
 * be checked by the application code via #ifdef ... #endif blocks.
 *
 */

#ifndef OPENGL_DEBUG_H
#define OPENGL_DEBUG_H

#include <opengl_base.h>


/* Comment the following line to turn off OpenGL debugging. */
#define OPENGL_DEBUG


#ifdef OPENGL_DEBUG

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
void APIENTRY opengl_debug_callback (GLenum source, 
                                     GLenum type, 
                                     GLuint id,
                                     GLenum severity, 
                                     GLsizei length, 
                                     const GLchar* message, 
                                     void* userParam);

#endif /* OPENGL_DEBUG */
#endif /* OPENGL_DEBUG_H */
