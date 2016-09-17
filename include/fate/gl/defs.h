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

/*! \file fate/gl/defs.h
 *  \brief Common definitions for the OpenGL backend.
 *  \ingroup gl
 *
 *  TODO
 *
 *  @{
 */

#ifndef FE_GL_DEFS
#define FE_GL_DEFS

#include <fate/defs.h>

#if __DOXYGEN__
/*! \brief TODO */
#define FE_GL_USE_GLAD 1
/*! \brief TODO */
#define FE_GL_USE_GLEW 1
#endif

#if 0 /* defined(FE_TARGET_EMSCRIPTEN) */
#define FE_GL_USE_GLEW 1
#elif !defined(FE_TARGET_EMSCRIPTEN)
#define FE_GL_USE_GLAD 1
#else
#define FE_GL_USE_GLEW 1
#endif

/*! \brief TODO 
 *
 * This macro exists to wrap the behaviour of GLEW (or GLAD)
 *
 * Example :
 * fe_gl_has(KHR_debug);
 *
 * \param ext The GL extension name, without the GL_ suffix.
 */
#ifdef FE_GL_USE_GLAD
#define fe_gl_has(ext) (GLAD_GL_##ext)
#elif defined(FE_GL_USE_GLEW)
#define fe_gl_has(ext) (GLEW_##ext)
#else
#define fe_gl_has(ext) (0)
#endif

#if __DOXYGEN__
/*! \brief TODO */
#define FE_GL_TARGET_DESKTOP 1
/*! \brief TODO */
#define FE_GL_TARGET_ES 1
#endif

#ifdef FE_TARGET_ENV_DESKTOP
#define FE_GL_TARGET_DESKTOP 1
#else
#define FE_GL_TARGET_ES 1
#endif


/*! \brief TODO */
#define GL_GLEXT_PROTOTYPES
#ifdef FE_GL_USE_GLAD
    #include <glad/glad.h>
    #define GLAPIENTRY APIENTRY
#elif defined(FE_GL_USE_GLEW)
    #include <GL/glew.h>
#endif
/*! \brief TODO */
#define BUFFER_OFFSET(_i_) ((void*)_i_)


/*! @} */
#endif /* FE_GL_DEFS */
