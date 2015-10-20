/**
 * @file glmake.h
 * @author Yoan Lecoq
 * @date 11 Jul 2015
 * @brief Definition of the glmake interface.
 *
 * “Sometimes, the elegant implementation is just a function. Not a method. Not a class. Not a framework. Just a function.” – John Carmack
 */

#ifndef GLMAKE_H
#define GLMAKE_H

#include <stdint.h>
#include <stdarg.h>
#include <opengl_base.h>

/**
 * @brief A helper function to create OpenGL programs, in a 'make' fashion.
 *
 * glmake() is intended as an efficient way to build OpenGL programs at run-time, quite 
 * like the well-known 'make' utility.
 * glmake() builds a program binary file if :
 *  -> It doesn't exist;
 *  -> Or it is incompatible;
 *  -> Or one of the required shader file's last write time is greater.
 * Otherwise, the existing program binary is used and no other operation takes place.
 * Shader objects compiled in the process are stored in a table with their hashed 
 * filename as a key, and reused when needed for next calls to glmake().
 * The program object is then linked.
 *
 * It is the caller's responsibility to :
 * -> Create the program object via glCreateProgram();
 * -> Set any pre-linking parameters to the program object before calling glmake();
 * -> Delete the program object via glDeleteProgram();
 * -> Free glmake()'s tables by calling glmake_clean().
 *
 * Useful link : https://www.opengl.org/wiki/Shader_Compilation
 *
 * glmake()'s prototype must be considered as follows :
 * int glmake(unsigned flags, GLuint program, 
 *     const char *program_binary_save_path, 
 *     const char *shader_path_1,
 *     const char *shader_path_2,
 *     ...
 *     const char *shader_path_n,
 *     NULL);
 * 'program_binary_save_path' indicates where to save the program's binary data. If the file already exists, and no 'shader_path' file outdates it, and its data is compatible with the hardware, then its data is imported into @p program, a message is issued, and no other operation takes place.
 * In case 'program_binary_save_path' exists but is not compatible, a warning is issued and the process continues as if it didn't exist.
 * Any number of 'shader_path' may be supplied, provided that the end of the list is indicated by a NULL pointer. Any empty string is just ignored, which can be useful for automatic use.
 * For each 'shader_path' :
 *     if its hash is already in the database, then the shader object associated with it is used and no compilation takes place. Otherwise, it is read into a newly created OpenGL shader object, compiled, stored in the database and finally attached to @p program. The shader's type is determined by the 'shader_path''s file extension, which can be one of ".vert", ".frag", ".geom", ".tesc", ".tese", and ".comp". 
 *     If the shader's type could not be determined or the compilation has failed, a detailed error message is issued and glmake()'s return value is set to 0.
 *
 * @p program is then linked against its shaders, and then its shaders are detached from it.
 *
 * On success, glmake() returns 1. Otherwise, 0 is returned.
 *
 * Basic usage :
 * @code
 * GLuint program = glCreateProgram();
 * //Set any pre-linking parameters here.
 * glmake(program, "cube.bin", "cube.vert", "cube.frag", NULL);
 * glmake_clean();
 * glUseProgram(program);
 * ...
 * glDeleteProgram(program);
 * @endcode
 */
int glmake(GLuint program, const char *save_path, ...);

/** @brief Clean-up function for glmake().
 *
 * glmake_clean() calls glDeleteShader() on each shader object stored 
 * previously by glmake(), and then empties the table. 
 * It is safe to call even if the table is already empty, and does not cause 
 * harm to future calls to glmake() either.
 */
void glmake_clean(void);


/* The following are globals and should never be interacted with. */

struct glmake_shaders_db_entry {
    uint64_t path_hash;
    GLuint shader_id;
};

struct glmake_shaders_db {
    struct glmake_shaders_db_entry *entries;
    unsigned top;
};

extern struct glmake_shaders_db glmake_shaders_db;

struct glmake_programs_db_entry {
    GLuint program_id;
    char *save_path;
};

struct glmake_programs_db {
    struct glmake_programs_db_entry *entries;
    unsigned top;
};

extern struct glmake_programs_db glmake_programs_db;

uint64_t glmake_hashfunc(const char *str);

#endif /* GLMAKE_H */ 
