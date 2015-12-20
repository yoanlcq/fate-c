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

void fate_gl_debug_setup(GLint gl_major, GLint gl_minor, bool enable);

#ifdef FATE_DEBUG_BUILD

#define FATE_GL_DEBUG

void GLAPIENTRY fate_gl_debug_msg_callback(GLenum source, GLenum type, 
                                GLuint id, GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                const void* userParam);


void GLAPIENTRY fate_glDebugMessageCallback_dummy(GLDEBUGPROC callback, 
                                       const void *userParam);
extern PFNGLDEBUGMESSAGECALLBACKPROC fate_glDebugMessageCallback;

void GLAPIENTRY fate_glDebugMessageControl_dummy(GLenum source, GLenum type, 
                                      GLenum severity, GLsizei count, 
                                      const GLuint *ids, 
                                      GLboolean enabled);
extern PFNGLDEBUGMESSAGECONTROLPROC fate_glDebugMessageControl;

void GLAPIENTRY fate_glDebugMessageInsertAMD(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message);
void GLAPIENTRY fate_glDebugMessageInsert_dummy(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message);
extern PFNGLDEBUGMESSAGEINSERTPROC fate_glDebugMessageInsert;

GLuint GLAPIENTRY fate_glGetDebugMessageLog_dummy(GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog);
extern PFNGLGETDEBUGMESSAGELOGPROC fate_glGetDebugMessageLog;

void GLAPIENTRY fate_glPushDebugGroup_dummy(GLenum source, GLuint id, GLsizei length, 
                                const char *message);
extern PFNGLPUSHDEBUGGROUPPROC fate_glPushDebugGroup;

void GLAPIENTRY fate_glPopDebugGroup_dummy(void);
extern PFNGLPOPDEBUGGROUPPROC fate_glPopDebugGroup;

void GLAPIENTRY fate_glObjectLabel_dummy(GLenum identifier, GLuint name, GLsizei length, 
                             const char *label);
extern PFNGLOBJECTLABELPROC fate_glObjectLabel;

void GLAPIENTRY fate_glObjectPtrLabel_dummy(const void *ptr, GLsizei length, 
                                const GLchar *label);
extern PFNGLOBJECTPTRLABELPROC fate_glObjectPtrLabel;

void GLAPIENTRY fate_glGetObjectLabel_dummy(GLenum identifier, GLuint name, 
                                GLsizei bufSize,
                                GLsizei *length, char *label);
extern PFNGLGETOBJECTLABELPROC fate_glGetObjectLabel;

void GLAPIENTRY fate_glGetObjectPtrLabel_dummy(const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label);
extern PFNGLGETOBJECTPTRLABELPROC fate_glGetObjectPtrLabel;


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

#endif /* FATE_DEBUG_BUILD */

#endif /* FATE_GL_DEBUG_H */
