/**
 * @file mkprog.h
 * @author Yoan Lecoq
 * @date 11 Jul 2015
 * @brief An interface to build and use OpenGL programs efficiently.
 *
 * “Sometimes, the elegant implementation is just a function. Not a method. Not a class. Not a framework. Just a function.” – John Carmack
 */

#ifndef FATE_GL_MKPROG_H
#define FATE_GL_MKPROG_H

#include <stdint.h>
#include <stdarg.h>
#include <fate/gl/defs.h>

/**
 * @brief A helper function to create OpenGL programs, in a 'make' fashion.
 *
 * fate_gl_mkprog() is intended as an efficient way to build OpenGL programs 
 * at run-time, quite like the well-known 'make' utility.
 * fate_gl_mkprog() builds a program binary file if at least one of the
 * following conditions are met :
 *  -> It doesn't exist;
 *  -> It is incompatible;
 *  -> One of the required shader file's last write time is greater.
 * Otherwise, the existing program binary is used and no other operation 
 * takes place.
 * Shader objects compiled in the process are stored in a table with their
 * hashed filename as a key, and reused when needed for next calls to
 * fate_gl_mkprog().
 * The program object is then linked.
 *
 * It is the caller's responsibility to :
 * -> Create the program object via glCreateProgram();
 * -> Set any pre-linking parameters to the program object before calling 
 *    fate_gl_mkprog();
 * -> Delete the program object via glDeleteProgram();
 * -> Yield resources by calling fate_gl_mkprog_yield().
 *
 *
 * Useful link : https://www.opengl.org/wiki/Shader_Compilation
 *
 *
 * fate_gl_mkprog()'s prototype must be considered as follows :
 *
 * int fate_gl_mkprog(unsigned flags, GLuint program, 
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
 *     failed, a detailed error message is issued and fate_gl_mkprog()'s 
 *     return value is set to 0.
 *
 * @p program is then linked against its shaders, and then its shaders are
 * detached from it.
 *
 * fate_gl_mkprog() returns 0 in case of a fatal error, and 1 otherwise.
 *
 * Basic usage :
 * @code
 * GLuint program = glCreateProgram();
 * //Set any pre-linking parameters here.
 * fate_gl_mkprog(program, "cube.bin", "cube.vert", "cube.frag", NULL);
 * fate_gl_mkprog_yield();
 * glUseProgram(program);
 * ...
 * glDeleteProgram(program);
 * @endcode
 */
int fate_gl_mkprog(GLuint program, const char *save_path, ...);

/** @brief Clean-up function for fate_gl_mkprog().
 *
 * fate_gl_mkprog_yield() calls glDeleteShader() on each shader object stored 
 * previously by fate_gl_mkprog(), and then empties the table. 
 * It is safe to call even if the table is already empty, and does not cause 
 * harm to future calls to fate_gl_mkprog() either.
 */
void fate_gl_mkprog_yield(void);


/* The following are globals and should never be interacted with. */

struct fate_gl_mkprog_shaders_db_entry {
    uint64_t path_hash;
    GLuint shader_id;
};

struct fate_gl_mkprog_shaders_db {
    struct fate_gl_mkprog_shaders_db_entry *entries;
    unsigned top;
};

extern struct fate_gl_mkprog_shaders_db fate_gl_mkprog_shaders_db;

struct fate_gl_mkprog_programs_db_entry {
    GLuint program_id;
    char *save_path;
};

struct fate_gl_mkprog_programs_db {
    struct fate_gl_mkprog_programs_db_entry *entries;
    unsigned top;
};

extern struct fate_gl_mkprog_programs_db fate_gl_mkprog_programs_db;

uint64_t fate_gl_mkprog_hashfunc(const char *str);

#endif /* FATE_GL_MKPROG_H */ 
