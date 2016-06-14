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

#ifndef FE_GL_DBG_H
#define FE_GL_DBG_H

#include <fate/gl/defs.h>
#include <stdbool.h>

void fe_gl_dbg_setup(GLint gl_major, GLint gl_minor, bool enable);

#ifdef FE_DEBUG_BUILD
#define FE_GL_DBG
#endif

#ifdef FE_GL_DBG

extern void (*fe_gl_dbg_insert_marker)(const char *string);
extern PFNGLDEBUGMESSAGECALLBACKPROC fe_gl_dbg_glDebugMessageCallback;
extern PFNGLDEBUGMESSAGECONTROLPROC fe_gl_dbg_glDebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC fe_gl_dbg_glDebugMessageInsert;
extern PFNGLGETDEBUGMESSAGELOGPROC fe_gl_dbg_glGetDebugMessageLog;
extern PFNGLPUSHDEBUGGROUPPROC fe_gl_dbg_glPushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC fe_gl_dbg_glPopDebugGroup;
extern PFNGLOBJECTLABELPROC fe_gl_dbg_glObjectLabel;
extern PFNGLOBJECTPTRLABELPROC fe_gl_dbg_glObjectPtrLabel;
extern PFNGLGETOBJECTLABELPROC fe_gl_dbg_glGetObjectLabel;
extern PFNGLGETOBJECTPTRLABELPROC fe_gl_dbg_glGetObjectPtrLabel;

#else /* FE_GL_DBG */

#define fe_gl_dbg_gl_debug_insert_marker(...)
#define fe_gl_dbg_glDebugMessageCallback(...)
#define fe_gl_dbg_glDebugMessageControl(...)
#define fe_gl_dbg_glDebugMessageInsert(...)
#define fe_gl_dbg_glGetDebugMessageLog(...)
#define fe_gl_dbg_glPushDebugGroup(...)
#define fe_gl_dbg_glPopDebugGroup()
#define fe_gl_dbg_glObjectLabel(...)
#define fe_gl_dbg_glObjectPtrLabel(...)
#define fe_gl_dbg_glGetObjectLabel(...)
#define fe_gl_dbg_glGetObjectPtrLabel(...)

#endif /* FE_DEBUG_BUILD */

#endif /* FE_GL_DBG_H */
