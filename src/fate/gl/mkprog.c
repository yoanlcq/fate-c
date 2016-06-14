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
#include <fate/fs.h>
#include <fate/log.h>
#include <fate/io.h>
#include <fate/mem.h>
#include <fate/gl.h>

static const char *TAG = "fe_gl";

struct fgm_shaders_db_entry {
    uint64_t hash;
    GLuint shader_id;
};
typedef struct fgm_shaders_db_entry fgm_shaders_db_entry;

struct fgm_shaders_db_struct {
    fgm_shaders_db_entry *entries;
    size_t top;
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
    fgm_shaders_db.entries = fe_mem_xrealloc(
        fgm_shaders_db.entries,
        fgm_shaders_db.top,
        fgm_shaders_db_entry,
        "fe_gl_mkprog shaders DB"
    );

    unsigned i;
    for(i=0 ; i<fgm_shaders_db.top-1 ; ++i) {
#ifdef FE_DEBUG_BUILD
        if(en->hash == fgm_shaders_db.entries[i].hash) {
            fe_loge(TAG, "Hash collision detected in the shaders hashtable.\n"
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
 * fe_gl_mkprog_setup().
 */
static const fgm_shader_type_entry shader_types_db_actual[] = {
#ifndef FE_EMSCRIPTEN
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
#ifdef FE_DEBUG_BUILD
        fe_loge(TAG, "\"%s\" has no file extension, "
                        "which is required to determine the shader type.\n", 
                        path);
#endif
        return GL_INVALID_ENUM;
    }
    ++file_extension; /* Skip '.' */

    const fgm_shader_type_entry *it;
    for(it=shader_types_db ; it->file_extension ; ++it)
        if(!strcmp(file_extension, it->file_extension))
            return it->shader_type;

#ifdef FE_DEBUG_BUILD
    fe_loge(TAG, "Could not determine \"%s\"'s shader type.\n"
                    "With the current OpenGL version, "
                    "recognized file extensions are :\n\t",
                    path);

    for(it=shader_types_db ; ; ++it) {
        fe_loge(TAG, ".%s", it->file_extension);
        if((it+2)->file_extension)
            fe_loge(TAG, ", ");
        else break;
    }
    fe_loge(TAG, "and .%s.\n", (it+1)->file_extension);
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

    fe_io file;
    fe_io___zallocv(&file, 1);
    fe_io_load_file(file, path, 0);
    if(!fe_io_ok(file)) {
        fe_loge(TAG, "Could not open \"%s\" : %s.\n", path, 
                fe_io_status_str(fe_io_get_status(file)));
        return 0;
    }
    fe_io_putc(file, '\0');
    GLuint shid = glCreateShader(shtype);
    fe_gl_dbg_glObjectLabel(GL_SHADER, shid, -1, path);
    fe_logv(TAG, "Compiling \"%s\"...\n", path); 

    GLchar *shsrc = fe_io_getdata(file);
   
#ifdef FE_EMSCRIPTEN
    glShaderSource(shid, 1, (const GLchar**) &shsrc, NULL);
#else
    glShaderSource(shid, 1, (const GLchar* const*) &shsrc, NULL);
#endif
    fe_io_close(file);
    fe_io___deallocv(&file, 1);

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
    fe_loge(TAG, "Could not compile \"%s\" :\n\t", path);
    fe_gl_log_shader_info(shid, fe_loge);
    fe_loge(TAG, "\n");
    glDeleteShader(shid);
    return 0;
}

static void fe_gl_mkprog_cleanup_2_0(void) 
{
    if(!fgm_shaders_db.entries)
        return;

    unsigned i;
    for(i=0 ; i<fgm_shaders_db.top ; ++i)
        glDeleteShader(fgm_shaders_db.entries[i].shader_id);

    fe_mem_free(fgm_shaders_db.entries);
    fgm_shaders_db.entries = NULL;
    fgm_shaders_db.top = 0;
}

static void fe_gl_mkprog_cleanup_4_1(void) 
{
    glReleaseShaderCompiler();
    fe_gl_mkprog_cleanup_2_0();
}

void (*fe_gl_mkprog_cleanup)(void);


static inline bool fgm_file_is_outdated(const char *path, va_list ap) {
    uint64_t mtime = fe_fs_get_last_write_time(path);
    for(;;) {
        path = va_arg(ap, const char*);
        if(!path) 
            break;
        if(!path[0])
            continue;
        if(fe_fs_get_last_write_time(path) > mtime)
            return true;
    }
    return false;
}

static bool fgm_program_from_binary(GLuint program, fe_io binfile, 
                                    const char *save_path) {
    size_t binlen = fe_io_getsize(binfile);
    char *bin = fe_io_getdata(binfile);
    GLenum binfmt = *(const GLenum*)bin;

    glProgramBinary(program, binfmt, bin+sizeof(GLenum), 
                    (GLsizei) (binlen-sizeof(GLenum)));

    GLint err, status;
    err = glGetError();
    if(err != GL_INVALID_ENUM) {
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if(status == GL_TRUE) {
            fe_logv(TAG, "Successfully reused \"%s\".\n", save_path);
            return true;
        }
    }
    fe_logw(TAG, "Could not reuse \"%s\" : \n\t", save_path);
    if(err != GL_INVALID_ENUM) {
        fe_gl_log_program_info(program, fe_logw);
        fe_logw(TAG, "\n");
    } else
        fe_logw(TAG, "Either 0x%x is an unrecognized binary format, "
                        "or the OpenGL implementation just rejected it. "
                        "See also the context's settings.\n", binfmt);
    return false;
}

static void fgm_program_to_binary(GLuint program, fe_io binfile, const char *binfile_path) {
    GLsizei binlen;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binlen);
    char *bin = fe_mem_malloc(binlen, char, "fe_gl prog binary");
    GLenum binfmt;
    glGetProgramBinary(program, binlen, NULL, &binfmt, bin);
    fe_io_write(binfile, &binfmt, sizeof(GLenum));
    fe_io_write(binfile, bin, binlen);
    fe_loader_wait(fe_io_sync_userdata(binfile, binfile_path, 0));
    fe_logv(TAG, "Saved binary with format 0x%x.\n", binfmt);
}


static bool fe_gl_mkprog_2_0_real(GLuint program, const char *save_path, 
                                   va_list ap)
{
    const char *path;
    bool all_good = true;
    GLuint shid;

    for(;;) {
        path = va_arg(ap, const char *);
        if(!path)
            break;
        if(!path[0])
            continue;
        shid = fgm_find_or_compile_shader(path);
        if(shid)
            glAttachShader(program, shid);
        else {
            fe_logw(TAG, "Could not compile/reuse \"%s\".\n", path);
            all_good = false;
        }
    }

    if(!all_good)
        return false;

    glLinkProgram(program);

    /* Detach all shaders before doing anything else */
    GLint num_shaders;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &num_shaders);
    GLuint *shaders = fe_mem_malloc(num_shaders, GLuint, 
        "fe_gl detach shaders"
    );
    glGetAttachedShaders(program, num_shaders, &num_shaders, shaders);
    for( ; num_shaders ; --num_shaders)
        glDetachShader(program, shaders[num_shaders-1]);
    fe_mem_free(shaders);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE) {
        fe_logv(TAG, "Successfully linked \"%s\".\n", save_path);
#ifdef FE_GL_DBG
        glValidateProgram(program);
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        fe_logv(TAG, "\"%s\" is %svalid. ", 
                        save_path, status==GL_TRUE ? "" : "in");
        fe_gl_log_program_info(program, fe_logv_not_macro);
        fe_logv(TAG, "\n");
#endif
        return true;
    }

    fe_loge(TAG, "Could not link \"%s\" :\n\t", save_path);
    fe_gl_log_program_info(program, fe_loge);
    fe_loge(TAG, "\n");
    return false;
}


static bool fe_gl_mkprog_4_1(GLuint program, const char *save_path, ...) {
    
    va_list ap;
    bool binfile_is_outdated=false;
    fe_io binfile;
    fe_io___zallocv(&binfile, 1);
    fe_io_load_file(binfile, save_path, 0);

    if(fe_io_ok(binfile)) {
        va_start(ap, save_path);
        binfile_is_outdated = fgm_file_is_outdated(save_path, ap);
        va_end(ap);
        if(!binfile_is_outdated) {
            if(fgm_program_from_binary(program, binfile, save_path)) {
                fe_io_close(binfile);
                fe_io___deallocv(&binfile, 1);
                return true;
            }
            binfile_is_outdated = true;
        }
        fe_io_close(binfile);
        fe_io___deallocv(&binfile, 1);
    }

    if(!fe_io_ok(binfile) || binfile_is_outdated) {
        glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, 
                            GL_TRUE);
        va_start(ap, save_path);
        int success = fe_gl_mkprog_2_0_real(program, save_path, ap);
        va_end(ap);
        if(!success)
            return false;
        fgm_program_to_binary(program, binfile, save_path);
        if(!fe_loader_wait(fe_io_sync_file(binfile, save_path, 0))) {
            fe_logw(TAG, "Could not save program binary to \"%s\".\n",
                            save_path);
            return true;
        }
        fe_io_close(binfile);
        fe_io___deallocv(&binfile, 1);
    }
    return true;
}

static bool fe_gl_mkprog_2_0(GLuint program, const char *save_path, ...) {
    va_list ap;
    va_start(ap, save_path);
    bool retval = fe_gl_mkprog_2_0_real(program, save_path, ap);
    va_end(ap);
    return retval;
}

bool (*fe_gl_mkprog)(GLuint program, const char *save_path, ...);

void fe_gl_mkprog_setup(GLint gl_major, GLint gl_minor) {

    unsigned version = gl_major*10 + gl_minor;

    if(version < 20) {
#ifdef FE_DEBUG_BUILD
        fe_logw(TAG, "fe_gl_mkprog is not available, because the OpenGL "
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
        fe_gl_mkprog = fe_gl_mkprog_4_1;
        fe_gl_mkprog_cleanup = fe_gl_mkprog_cleanup_4_1;
    } else {
        fe_gl_mkprog = fe_gl_mkprog_2_0;
        fe_gl_mkprog_cleanup = fe_gl_mkprog_cleanup_2_0;
    }
}
