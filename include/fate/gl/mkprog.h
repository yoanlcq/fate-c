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


/*! \file fate/gl/mkprog.h
 *  \brief An interface to build and use OpenGL programs efficiently.
 *  \ingroup gl
 *
 * “Sometimes, the elegant implementation is just a function. Not a method. Not a class. Not a framework. Just a function.” – John Carmack
 *
 * @{
 */

#ifndef FE_GL_MKPROG_H
#define FE_GL_MKPROG_H

#include <stdint.h>
#include <stdarg.h>
#include <fate/gl/defs.h>
#include <fate/iov.h>
#include <fate/timestamp.h>

/*! \brief TODO */
typedef struct {
    fe_iov_readonly vert; /*!< GL_VERTEX_SHADER */
    fe_iov_readonly frag; /*!< GL_FRAGMENT_SHADER */
    fe_iov_readonly tesc; /*!< GL_TESS_CONTROL_SHADER */
    fe_iov_readonly tese; /*!< GL_TESS_EVALUATION_SHADER */
    fe_iov_readonly geom; /*!< GL_GEOMETRY_SHADER */
    fe_iov_readonly comp; /*!< GL_COMPUTE_SHADER */
} fe_gl_shader_source_set;

/*! \brief TODO */
void fe_gl_mkprog_setup(GLint gl_major, GLint gl_minor);

/*! \brief A helper function to create OpenGL programs, in a 'make' fashion.
 *
 * fe_gl_mkprog() is intended as an efficient way to build OpenGL programs 
 * at run-time, quite like the well-known 'make' utility.
 * fe_gl_mkprog() builds a program binary if at least one of the
 * following conditions are met :
 * - It doesn't exist;
 * - It is incompatible;
 * - \p last_build_time is greater than the binary's last build time.
 * Otherwise, the existing program binary is used and no other operation 
 * takes place.
 * Shader objects compiled in the process are stored in a table with their
 * hashed source as a key, and reused when needed for next calls to
 * #fe_gl_mkprog().
 * The program object is then linked and shaders are detached from it.
 *
 * The so-called program binary is layed out like so :
 * - A 64-bit integer (fe_timestamp) holding the time (down to the second) 
 *   at which the binary was built.
 * - A 32-bit integer (GLenum) identifying the vendor-specific binary data.
 * - Vendor-specific binary data.
 *
 * Each member of the \p ss matches a shader stage.
 * For each of them :
 * - If the \c base pointer is \c NULL, the shader stage is ignored.
 * - Otherwise, and if \c len is zero, then the \c base pointer is assumed
 *   to point to a NULL-terminated string containing the whole shader stage's 
 *   source.
 * - Otherwise, the \c base pointer is assumed to point to an array of \c len 
 *   characters which should contain the whole shader stage's source.
 *
 * It is the caller's responsibility to :
 * - Create the program object via glCreateProgram();
 * - Set any pre-linking parameters to the program object before calling 
 *    #fe_gl_mkprog();
 * - Delete the program object via glDeleteProgram();
 * - Clean-up resources by calling #fe_gl_mkprog_cleanup().
 *
 * Useful link : https://www.opengl.org/wiki/Shader_Compilation
 *
 * fe_gl_mkprog() returns \c false in case of a fatal error, and \c true otherwise.
 *
 * Basic usage :
 * @code
 * fe_iov progbin = {0};
 * fe_iov_load_persistent(&progbin, "myapp", "cache/tri.glb");
 * GLuint prog = glCreateProgram();
 * fe_gl_shader_source_set ss = {{0}};
 * ss.vert.base = fe_gl_src_tri_330_vert;
 * ss.frag.base = fe_gl_src_tri_330_frag;
 * fe_gl_mkprog(prog, fe_gl_src_get_build_timestamp(), &progbin, &ss);
 * fe_iov_store_persistent(&progbin, "myapp", "cache/tri.glb");
 * fe_gl_mkprog_cleanup();
 * glUseProgram(program);
 * //Do stuff...
 * glDeleteProgram(program);
 * @endcode
 */
extern bool (*fe_gl_mkprog)(
    GLuint progid,
    fe_timestamp last_build_time,
    fe_iov *restrict progbin,
    const fe_gl_shader_source_set *restrict ss
);

/*! \brief TODO */
#define fe_gl_mkprog_no_binary(progid, ss) \
        fe_gl_mkprog(progid, 0, NULL, ss)

/*! \brief Clean-up function for fe_gl_mkprog().
 *
 * fe_gl_mkprog_cleanup() calls glDeleteShader() on each shader object stored 
 * previously by fe_gl_mkprog(), and then empties the table. 
 * It is safe to call even if the table is already empty, and does not cause 
 * harm to future calls to fe_gl_mkprog() either.
 */
extern void (*fe_gl_mkprog_cleanup)(void);

/*! @} */
#endif /* FE_GL_MKPROG_H */ 
