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
#endif

#ifdef FATE_GL_DEBUG

extern void (*fate_gl_debug_insert_marker)(const char *string);
extern PFNGLDEBUGMESSAGECALLBACKPROC fate_glDebugMessageCallback;
extern PFNGLDEBUGMESSAGECONTROLPROC fate_glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC fate_glDebugMessageInsert;
extern PFNGLGETDEBUGMESSAGELOGPROC fate_glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC fate_glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC fate_glPopDebugGroup;
extern PFNGLOBJECTLABELPROC fate_glObjectLabel;
extern PFNGLOBJECTPTRLABELPROC fate_glObjectPtrLabel;
extern PFNGLGETOBJECTLABELPROC fate_glGetObjectLabel;
extern PFNGLGETOBJECTPTRLABELPROC fate_glGetObjectPtrLabel;

#else /* FATE_GL_DEBUG */

#define fate_gl_debug_insert_marker(...)
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
