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
 *
 * “Sometimes, the elegant implementation is just a function. Not a method. Not a class. Not a framework. Just a function.” – John Carmack
 */

#ifndef FE_GL_MKPROG_H
#define FE_GL_MKPROG_H

#include <stdint.h>
#include <stdarg.h>
#include <fate/gl/defs.h>

void fe_gl_mkprog_setup(GLint gl_major, GLint gl_minor);

/**
 * @brief A helper function to create OpenGL programs, in a 'make' fashion.
 *
 * fe_gl_mkprog() is intended as an efficient way to build OpenGL programs 
 * at run-time, quite like the well-known 'make' utility.
 * fe_gl_mkprog() builds a program binary file if at least one of the
 * following conditions are met :
 *  -> It doesn't exist;
 *  -> It is incompatible;
 *  -> One of the required shader file's last write time is greater.
 * Otherwise, the existing program binary is used and no other operation 
 * takes place.
 * Shader objects compiled in the process are stored in a table with their
 * hashed filename as a key, and reused when needed for next calls to
 * fe_gl_mkprog().
 * The program object is then linked.
 *
 * It is the caller's responsibility to :
 * -> Create the program object via glCreateProgram();
 * -> Set any pre-linking parameters to the program object before calling 
 *    fe_gl_mkprog();
 * -> Delete the program object via glDeleteProgram();
 * -> Clean-up resources by calling fe_gl_mkprog_cleanup().
 *
 *
 * Useful link : https://www.opengl.org/wiki/Shader_Compilation
 *
 *
 * fe_gl_mkprog()'s prototype must be considered as follows :
 *
 * int fe_gl_mkprog(unsigned flags, GLuint program, 
 *     const char *program_binary_save_path, 
 *     const char *shader_path_1,
 *     const char *shader_path_2,
 *     ...
 *     const char *shader_path_n,
 *     NULL);
 *
 * 'program_binary_save_path' indicates where to save the program's binary.
 * If the file already exists, and no 'shader_path' file outdates it, and its
 * data is compatible with the current hardware/driver, then its data is
 * imported into @p program, a message is issued, and no other operation 
 * takes place.
 *
 * In case 'program_binary_save_path' exists but is not compatible, a 
 * warning is issued and the process continues as if it didn't exist.
 *
 * Any number of 'shader_path' may be supplied, provided that the end of 
 * the list is indicated by a NULL pointer. Any empty string is just 
 * ignored, which can be useful for automatic use.
 *
 * For each 'shader_path' :
 *     If its hash is already in the database, then the shader object
 *     associated with it is used and no compilation takes place.
 *     Otherwise, it is read into a newly created OpenGL shader object,
 *     compiled, stored in the database and finally attached to @p program. 
 *     
 *     The shader's type is determined by the file extension, which can be 
 *     one of ".vert", ".frag", ".geom", ".tesc", ".tese", and ".comp". 
 *     If the shader's type could not be determined or the compilation has 
 *     failed, a detailed error message is issued and fe_gl_mkprog()'s 
 *     return value is set to 0.
 *
 * @p program is then linked against its shaders, and then its shaders are
 * detached from it.
 *
 * fe_gl_mkprog() returns 0 in case of a fatal error, and 1 otherwise.
 *
 * Basic usage :
 * @code
 * fe_gl_mkprog_setup(gl_major, gl_minor);
 * GLuint program = glCreateProgram();
 * //Set any pre-linking parameters here.
 * fe_gl_mkprog(program, "cube.bin", "cube.vert", "cube.frag", NULL);
 * fe_gl_mkprog_cleanup();
 * glUseProgram(program);
 * ...
 * glDeleteProgram(program);
 * @endcode
 */
extern int (*fe_gl_mkprog)(GLuint program, const char *save_path, ...) FE_SENTINEL(0);

/** @brief Clean-up function for fe_gl_mkprog().
 *
 * fe_gl_mkprog_cleanup() calls glDeleteShader() on each shader object stored 
 * previously by fe_gl_mkprog(), and then empties the table. 
 * It is safe to call even if the table is already empty, and does not cause 
 * harm to future calls to fe_gl_mkprog() either.
 */
extern void (*fe_gl_mkprog_cleanup)(void);

#endif /* FE_GL_MKPROG_H */ 
