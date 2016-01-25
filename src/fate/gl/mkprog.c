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

/*! \file fate/gl/mkprog.c
 *  \brief Efficiently creating and reusing OpenGL programs.
 *
 * TODO
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fate/defs.h>
#include <fate/sys.h>
#include <fate/log.h>
#include <fate/fatal_alloc.h>
#include <fate/file_to_string.h>
#include <fate/gl/defs.h>
#include <fate/gl/log.h>
#include <fate/gl/debug.h>
#include <fate/gl/mkprog.h>

struct fgm_shaders_db_entry {
    uint64_t hash;
    GLuint shader_id;
};
typedef struct fgm_shaders_db_entry fgm_shaders_db_entry;

struct fgm_shaders_db_struct {
    fgm_shaders_db_entry *entries;
    unsigned top;
};
typedef struct fgm_shaders_db_struct fgm_shaders_db_struct;

static fgm_shaders_db_struct fgm_shaders_db = {NULL, 0};

/* 
 * This hash function is sbdm, from http://www.cse.yorku.ca/~oz/hash.html
 * It is public domain.
 */
static inline uint64_t sdbm(const char *str)
{
    int c;
    uint64_t hash = 0;
    while((c = *(++str)))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

static int fgm_shaders_db_entry_cmp(const void* a, const void* b) {
    const fgm_shaders_db_entry *left = a;
    const fgm_shaders_db_entry *rght = b;
    if(left->hash > rght->hash)
        return 1;
    if(left->hash < rght->hash)
        return -1;
    return 0;
}

static fgm_shaders_db_entry* fgm_find_shader_entry(uint64_t hash)
{
    fgm_shaders_db_entry key;

    if(!fgm_shaders_db.entries)
        return NULL;

    key.hash = hash;
    return bsearch(&key, fgm_shaders_db.entries, 
                   fgm_shaders_db.top, sizeof(fgm_shaders_db_entry),
                   fgm_shaders_db_entry_cmp);
}

static void fgm_add_shader_entry(const fgm_shaders_db_entry *en)
{
    ++(fgm_shaders_db.top);
    fgm_shaders_db.entries = fate_fatal_realloc(fgm_shaders_db.entries,
            fgm_shaders_db.top,
            sizeof(fgm_shaders_db_entry));

    unsigned i;
    for(i=0 ; i<fgm_shaders_db.top-1 ; ++i) {
#ifdef FATE_DEBUG_BUILD
        if(en->hash == fgm_shaders_db.entries[i].hash) {
            fate_logf_err("fate_gl_mkprog: Hash collision detected "
                          "in the shaders hashtable.\n"
                          "The hash is %#016llX.\n", 
                          (unsigned long long)en->hash);
            continue;
        }
#endif
        if(en->hash < fgm_shaders_db.entries[i].hash) {
            memmove(fgm_shaders_db.entries+i+1, fgm_shaders_db.entries+i, 
                    (fgm_shaders_db.top-i-1)*sizeof(fgm_shaders_db_entry));
            break;
        }
    }
    memcpy(fgm_shaders_db.entries+i, en, sizeof(fgm_shaders_db_entry));
}

struct fgm_shader_type_entry {
    const char *file_extension;
    GLenum shader_type;
    uint8_t min_gl_version;
};
typedef struct fgm_shader_type_entry fgm_shader_type_entry;

/* 
 * Must be sorted from latest GL version to older. 
 * The lower the GL version, the farther the start pointer is set by 
 * fate_gl_mkprog_setup().
 */
static const fgm_shader_type_entry shader_types_db_actual[] = {
#ifndef FATE_EMSCRIPTEN
    { "comp", GL_COMPUTE_SHADER,         43 },
#endif
    { "tesc", GL_TESS_CONTROL_SHADER,    40 },
    { "tese", GL_TESS_EVALUATION_SHADER, 40 },
    { "geom", GL_GEOMETRY_SHADER,        32 },
    { "vert", GL_VERTEX_SHADER,          20 },
    { "frag", GL_FRAGMENT_SHADER,        20 },
    { NULL,   GL_INVALID_ENUM,            0 }
};

static const fgm_shader_type_entry *shader_types_db = NULL;

static GLenum fgm_shader_type_from_extension(const char *path) {

    const char *file_extension = strrchr(path, '.');
    if(!file_extension) {
#ifdef FATE_DEBUG_BUILD
        fate_logf_video("\"%s\" has no file extension, "
                        "which is required to determine the shader type.\n", 
                        path);
#endif
        return GL_INVALID_ENUM;
    }
    ++file_extension; /* Skip '.' */

    const fgm_shader_type_entry *it;
    for(it=shader_types_db ; it->file_extension ; ++it)
        if(strcmp(file_extension, it->file_extension)==0)
            return it->shader_type;

#ifdef FATE_DEBUG_BUILD
    fate_logf_video("Could not determine \"%s\"'s shader type.\n"
                    "With the current OpenGL version, "
                    "recognized file extensions are :\n\t",
                    path);

    for(it=shader_types_db ; ; ++it) {
        fate_logf_video(".%s", it->file_extension);
        if((it+2)->file_extension)
            fate_logf_video(", ");
        else break;
    }
    fate_logf_video("and .%s.\n", (it+1)->file_extension);
#endif
    return GL_INVALID_ENUM;
}

GLuint fgm_find_or_compile_shader(const char *path) 
{
    GLenum shtype = fgm_shader_type_from_extension(path);
    if(shtype == GL_INVALID_ENUM) 
        return 0;

    /* Check hashtable */
    uint64_t hash = sdbm(path); /* reused later */
    fgm_shaders_db_entry *entry = fgm_find_shader_entry(hash);
    if(entry)
        return entry->shader_id;

    FILE *file = fopen(path, "r");
    if(!file) {
        fate_logf_video("Could not open \"%s\".\n", path);
        return 0;
    }
    GLuint shid = glCreateShader(shtype);
    fate_glObjectLabel(GL_SHADER, shid, -1, path);
    fate_logf_video("Compiling \"%s\"...\n", path); 

    size_t num_strings;
    char **shsrc = fate_file_to_string_array(file, &num_strings);
    fclose(file);
   
#ifdef FATE_EMSCRIPTEN
    glShaderSource(shid, num_strings, (const GLchar**) shsrc, NULL);
#else
    glShaderSource(shid, num_strings, (const GLchar* const*) shsrc, NULL);
#endif

    unsigned i;
    for(i=0 ; i<num_strings ; ++i)
        free(shsrc[i]);
    free(shsrc);

    glCompileShader(shid);

    fgm_shaders_db_entry res;
    GLint status;
    glGetShaderiv(shid, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE) {
        res.hash = hash;
        res.shader_id = shid;
        fgm_add_shader_entry(&res);
        return shid;
    }
    fate_logf_video("Could not compile \"%s\" :\n\t", path);
    fate_gl_log_shader_info(shid);
    fate_logf_video("\n");
    glDeleteShader(shid);
    return 0;
}

static void fate_gl_mkprog_cleanup_2_0(void) 
{
    if(!fgm_shaders_db.entries)
        return;

    unsigned i;
    for(i=0 ; i<fgm_shaders_db.top ; ++i)
        glDeleteShader(fgm_shaders_db.entries[i].shader_id);

    free(fgm_shaders_db.entries);
    fgm_shaders_db.entries = NULL;
    fgm_shaders_db.top = 0;
}

static void fate_gl_mkprog_cleanup_4_1(void) 
{
    glReleaseShaderCompiler();
    fate_gl_mkprog_cleanup_2_0();
}

void (*fate_gl_mkprog_cleanup)(void);


static inline bool fgm_file_is_outdated(const char *path, va_list ap) {
    uint64_t mtime = fate_sys_get_last_write_time(path);
    for(;;) 
    {
        path = va_arg(ap, const char*);
        if(path == NULL) 
            break;
        if(path[0] == '\0')
            continue;
        if(fate_sys_get_last_write_time(path) > mtime)
            return true;
    }
    return false;
}

static bool fgm_program_from_binary(GLuint program, FILE *binfile, 
                                    const char *save_path) {
    uint64_t binlen;
    char *bin = fate_file_to_string(binfile, &binlen);
    GLenum binfmt = *(GLenum*)bin;

    glProgramBinary(program, binfmt, bin+sizeof(GLenum), 
                    (GLsizei) (binlen-sizeof(GLenum)));
    free(bin);

    GLint err, status;
    err = glGetError();
    if(err != GL_INVALID_ENUM) {
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if(status == GL_TRUE) {
            fate_logf_video("Successfully reused \"%s\".\n", save_path);
            return true;
        }
    }
    fate_logf_video("Could not reuse \"%s\" : \n\t", save_path);
    if(err != GL_INVALID_ENUM) {
        fate_gl_log_program_info(program);
        fate_logf_video("\n");
    } else
        fate_logf_video("Either 0x%x is an unrecognized binary format, "
                        "or the OpenGL implementation just rejected it. "
                        "See also the context's settings.\n", binfmt);

    return false;
}

static void fgm_program_to_binary(GLuint program, FILE *binfile) {
    GLsizei binlen;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binlen);
    char *bin = malloc(binlen);
    GLenum binfmt;
    glGetProgramBinary(program, binlen, NULL, &binfmt, bin);
    fate_logf_video("Saved binary with format 0x%x.\n", binfmt);
    fwrite(&binfmt, sizeof(GLenum), 1, binfile);
    fwrite(bin, 1, binlen, binfile);
    free(bin);
}


static int fate_gl_mkprog_2_0_real(GLuint program, const char *save_path, 
                                   va_list ap)
{
    const char *path;
    int all_good = 1;
    GLuint shid;

    for(;;) {
        path = va_arg(ap, const char *);
        if(path == NULL)
            break;
        if(path[0] == '\0')
            continue;
        shid = fgm_find_or_compile_shader(path);
        if(shid)
            glAttachShader(program, shid);
        else {
            fate_logf_video("Could not compile/reuse \"%s\".\n", path);
            all_good = 0;
        }
    }

    if(!all_good)
        return 0;

    glLinkProgram(program);

    /* Detach all shaders before doing anything else */
    GLint num_shaders;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &num_shaders);
    GLuint *shaders = malloc(num_shaders*sizeof(GLuint));
    glGetAttachedShaders(program, num_shaders, &num_shaders, shaders);
    for( ; num_shaders ; --num_shaders)
        glDetachShader(program, shaders[num_shaders-1]);
    free(shaders);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) {
        fate_logf_video("Successfully linked \"%s\".\n", save_path);
#ifdef FATE_GL_DEBUG
        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        fate_logf_video("\"%s\" is %svalid. ", 
                        save_path, status==GL_TRUE ? "" : "in");
        fate_gl_log_program_info(program);
        fate_logf_video("\n");
#endif
        return 1;
    }

    fate_logf_video("Could not link \"%s\" :\n\t", save_path);
    fate_gl_log_program_info(program);
    fate_logf_video("\n");

    return 0;
}


static int fate_gl_mkprog_4_1(GLuint program, const char *save_path, ...) {
    
    va_list ap;
    bool binfile_is_outdated=false;
    FILE *binfile = fopen(save_path, "rb");

    if(binfile) {
        va_start(ap, save_path);
        binfile_is_outdated = fgm_file_is_outdated(save_path, ap);
        va_end(ap);
        if(!binfile_is_outdated) {
            if(fgm_program_from_binary(program, binfile, save_path)) {
                fclose(binfile);
                return 1;
            }
            binfile_is_outdated = true;
        }
        fclose(binfile);
    }

    if(!binfile || binfile_is_outdated) {
        glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, 
                            GL_TRUE);
        va_start(ap, save_path);
        int success = fate_gl_mkprog_2_0_real(program, save_path, ap);
        va_end(ap);
        if(!success)
            return 0;
        binfile = fopen(save_path, "wb");
        if(!binfile) {
            fate_logf_video("Could not save program binary to \"%s\".\n",
                            save_path);
            return 1;
        }
        fgm_program_to_binary(program, binfile);
        fclose(binfile);
    }
    return 1;
}

static int fate_gl_mkprog_2_0(GLuint program, const char *save_path, ...) {
    va_list ap;
    va_start(ap, save_path);
    int retval = fate_gl_mkprog_2_0_real(program, save_path, ap);
    va_end(ap);
    return retval;
}

int (*fate_gl_mkprog)(GLuint program, const char *save_path, ...);

void fate_gl_mkprog_setup(GLint gl_major, GLint gl_minor) {

    unsigned version = gl_major*10 + gl_minor;

    if(version < 20) {
#ifdef FATE_DEBUG_BUILD
        fate_logf_video("fate_gl_mkprog is not available, because the OpenGL "
                        "version is less than 2.0.\n"
                        "If you see this, something is wrong with "
                        "the caller's code.\n"); 
#endif
        return;
    }

    for(shader_types_db = shader_types_db_actual ; ; ++shader_types_db)
        if(shader_types_db->min_gl_version <= version)
            break;

    if(version >= 41) {
        fate_gl_mkprog = fate_gl_mkprog_4_1;
        fate_gl_mkprog_cleanup = fate_gl_mkprog_cleanup_4_1;
    } else {
        fate_gl_mkprog = fate_gl_mkprog_2_0;
        fate_gl_mkprog_cleanup = fate_gl_mkprog_cleanup_2_0;
    }
}
