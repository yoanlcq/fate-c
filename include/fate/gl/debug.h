/**
 * @file opengl_debug.h
 * @author Yoan Lecoq
 * @date 13 Jul 2015
 * @brief Header file for all things "OpenGL debug".
 *
 */

#ifndef FATE_GL_DEBUG_H
#define FATE_GL_DEBUG_H

#include <fate/gl/defs.h>
#include <stdbool.h>

#ifdef FATE_DEBUG_BUILD

#define FATE_GL_DEBUG

#ifndef APIENTRY /* Noticeable on Linux */
#define APIENTRY
#endif

/* See the OpenGL Red book. */
typedef void (APIENTRY *DEBUGPROC)  (GLenum source, 
                                     GLenum type, 
                                     GLuint id,
                                     GLenum severity, 
                                     GLsizei length, 
                                     const GLchar* message, 
                                     void* userParam);
void APIENTRY fate_gl_debug_msg_callback  (GLenum source, 
                                     GLenum type, 
                                     GLuint id,
                                     GLenum severity, 
                                     GLsizei length, 
                                     const GLchar* message, 
                                     void* userParam);

#define HELPER(_type_,_name_,_params_) \
    _type_ fate_##_name_##_dummy _params_; \
    extern _type_ (*fate_##_name_) _params_;

HELPER(void, glDebugMessageCallback, (DEBUGPROC callback, void *userParam));
HELPER(void, glDebugMessageControl, (GLenum source, GLenum type, 
            GLenum severity, GLsizei count, const GLuint *ids, 
            GLboolean enabled));
HELPER(void, glDebugMessageInsert, (GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message));
HELPER(GLuint,glGetDebugMessageLog, (GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog));
HELPER(void, glPushDebugGroup, (GLenum source, GLuint id, GLsizei length, 
                                const char *message));
HELPER(void, glPopDebugGroup, (void));
HELPER(void, glObjectLabel, (GLenum identifier, GLuint name, GLsizei length, 
                             const char *label));
HELPER(void, glObjectPtrLabel, (void *ptr, GLsizei length, const char *label));
HELPER(void, glGetObjectLabel, (GLenum identifier, GLuint name, GLsizei bufSize,
                                GLsizei *length, char *label));
HELPER(void, glGetObjectPtrLabel, (const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label));

#undef HELPER

void fate_gl_debug_setup(GLint gl_major, GLint gl_minor, bool enable);

#else /* FATE_DEBUG_BUILD */

#define fate_glDebugMessageCallback(...)
#define fate_glDebugMessageControl(...)
#define fate_glDebugMessageInsert(...)
#define fate_glGetDebugMessageLog(...)
#define fate_glPushDebugGroup(...)
#define fate_glPopDebugGroup()
#define fate_glObjectLabel(...)
#define fate_glObjectPtrLabel(...)
#define fate_glGetObjectLabel(...)
#define fate_glGetObjectPtrLabel(...)
#define fate_gl_debug_setup(...)

#endif /* FATE_DEBUG_BUILD */

#endif /* FATE_GL_DEBUG_H */
