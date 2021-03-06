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

/*! \file fate/gl/log.h
 *  \brief Utilities for logging parts of OpenGL's state.
 *  \ingroup gl
 * TODO
 *
 * @{
 */

#ifndef FE_GL_LOG
#define FE_GL_LOG

#include <stdbool.h>
#include <fate/gl/defs.h>

#ifdef GL_CHECK_ERRORS
#define GL_CHECK_ERR() print_gl_errors(stdout)
#else
/*! \brief TODO */
#define GL_CHECK_ERR()
#endif

#include <fate/log.h>

/*! \brief TODO */
void fe_gl_log_error(GLenum err, fe_logfunc log, bool even_no_error);
/*! \brief TODO */
void fe_gl_log_all_errors(fe_logfunc log);
/*! \brief TODO */
void fe_gl_log_program_info(GLuint program, fe_logfunc log);
/*! \brief TODO */
void fe_gl_log_shader_info(GLuint shader, fe_logfunc log);

/*! @} */
#endif /* FE_GL_LOG */
