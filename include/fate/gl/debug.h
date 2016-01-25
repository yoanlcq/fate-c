/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */


/*! \file fate/gl/debug.h
 *  \brief OpenGL debug management.
 *
 *  TODO
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
