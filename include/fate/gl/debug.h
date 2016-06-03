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

#ifndef FE_GL_DEBUG_H
#define FE_GL_DEBUG_H

#include <fate/gl/defs.h>
#include <stdbool.h>

void fe_gl_debug_setup(GLint gl_major, GLint gl_minor, bool enable);

#ifdef FE_DEBUG_BUILD
#define FE_GL_DEBUG
#endif

#ifdef FE_GL_DEBUG

extern void (*fe_gl_debug_insert_marker)(const char *string);
extern PFNGLDEBUGMESSAGECALLBACKPROC fe_glDebugMessageCallback;
extern PFNGLDEBUGMESSAGECONTROLPROC fe_glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC fe_glDebugMessageInsert;
extern PFNGLGETDEBUGMESSAGELOGPROC fe_glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC fe_glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC fe_glPopDebugGroup;
extern PFNGLOBJECTLABELPROC fe_glObjectLabel;
extern PFNGLOBJECTPTRLABELPROC fe_glObjectPtrLabel;
extern PFNGLGETOBJECTLABELPROC fe_glGetObjectLabel;
extern PFNGLGETOBJECTPTRLABELPROC fe_glGetObjectPtrLabel;

#else /* FE_GL_DEBUG */

#define fe_gl_debug_insert_marker(...)
#define fe_glDebugMessageCallback(...)
#define fe_glDebugMessageControl(...)
#define fe_glDebugMessageInsert(...)
#define fe_glGetDebugMessageLog(...)
#define fe_glPushDebugGroup(...)
#define fe_glPopDebugGroup()
#define fe_glObjectLabel(...)
#define fe_glObjectPtrLabel(...)
#define fe_glGetObjectLabel(...)
#define fe_glGetObjectPtrLabel(...)

#endif /* FE_DEBUG_BUILD */

#endif /* FE_GL_DEBUG_H */
