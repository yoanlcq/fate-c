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
#include <string.h>
#include <inttypes.h>
#include <fate/defs.h>
#include <fate/log.h>
#include <fate/iov.h>
#include <fate/mem.h>
#include <fate/gl.h>
#include <fate/hash.h>
#include <fate/timestamp.h>
#include <fate/hw.h>

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
                    "The hash is %#016"PRIx64".\n", 
                    en->hash);
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
#ifndef FE_TARGET_EMSCRIPTEN
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

GLuint fgm_find_or_compile_shader(const fe_iov_readonly *src, GLenum shtype) 
{
    /* Check hashtable */
    uint64_t hash = fe_hash_sdbm(src->base, src->len ? src->len : ~(size_t)0);
    fgm_shaders_db_entry *entry = fgm_find_shader_entry(hash);
    if(entry)
        return entry->shader_id;

    GLuint shid = glCreateShader(shtype);
    /* fe_gl_dbg_glObjectLabel(GL_SHADER, shid, -1, path); XXX */
    /* fe_logv(TAG, "Compiling \"%s\"...\n", path); XXX */

    GLint lenarg = src->len ? src->len : -1;
#ifdef FE_TARGET_EMSCRIPTEN
    glShaderSource(shid, 1, (const GLchar**) &src->base, &lenarg);
#else
    glShaderSource(shid, 1, (const GLchar* const*) &src->base, &lenarg);
#endif

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
    /* fe_loge(TAG, "Could not compile \"%s\" :\n\t", path); XXX */
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
    glReleaseShaderCompiler(); // >= ES 2.0
    fe_gl_mkprog_cleanup_2_0();
}

void (*fe_gl_mkprog_cleanup)(void);

static const size_t header_size = sizeof(fe_timestamp) + sizeof(GLenum);

static bool fgm_program_from_binary(GLuint program, fe_iov *binfile) {
#ifndef FE_TARGET_EMSCRIPTEN
    GLsizei binlen = binfile->len - header_size;
    char *bin = ((char*)binfile->base) + header_size;
    GLenum binfmt = fe_hw_swap32_net_to_host(
        *(GLenum*)(((char*)binfile->base)+sizeof(fe_timestamp))
    );

    glProgramBinary(program, binfmt, bin, binlen); // >= ES 3.0

    GLint err, status;
    err = glGetError();
    if(err != GL_INVALID_ENUM) {
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if(status == GL_TRUE) {
            /* fe_logv(TAG, "Successfully reused \"%s\".\n", save_path); XXX */
            return true;
        }
    }
    /* fe_logw(TAG, "Could not reuse \"%s\" : \n\t", save_path); XXX */
    if(err != GL_INVALID_ENUM) {
        fe_gl_log_program_info(program, fe_logw);
        fe_logw(TAG, "\n");
    } else
        fe_logw(TAG, "Either 0x%x is an unrecognized binary format, "
                        "or the OpenGL implementation just rejected it. "
                        "See also the context's settings.\n", binfmt);
    return false;
#else /* FE_TARGET_EMSCRIPTEN */
    return true;
#endif
}

static void fgm_program_to_binary(GLuint program, fe_iov *binfile) {
#ifndef FE_TARGET_EMSCRIPTEN
    GLsizei binlen;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binlen);
    binfile->len = header_size + binlen;
    binfile->base = fe_mem_malloc(binfile->len, char, "fe_gl prog binary");
    GLenum binfmt;
    glGetProgramBinary(program, binlen, NULL, &binfmt, ((char*)binfile->base)+header_size); // >= ES 3.0
    *(fe_timestamp*)binfile->base = fe_hw_swap64_host_to_net(fe_timestamp_get_now());
    *(GLenum*)(((char*)binfile->base)+sizeof(fe_timestamp)) = fe_hw_swap32_host_to_net(binfmt);
    fe_logv(TAG, "Saved binary with format 0x%x.\n", binfmt);
#endif /* FE_TARGET_EMSCRIPTEN */
}

static bool fe_gl_mkprog_2_0(GLuint program, 
                             fe_timestamp last_build_time,
                             fe_iov *restrict progbin,
                             const fe_gl_shader_source_set *restrict ss)
{
    GLuint shid;
#define TRY_STAGE(stage, stage_enum) \
    if(ss->stage.base) { \
        shid = fgm_find_or_compile_shader(&ss->stage, stage_enum); \
        if(shid) \
            glAttachShader(program, shid); \
        else { \
            fe_logw(TAG, "Could not compile/reuse \"%s\".\n", #stage_enum);\
            return false; \
        } \
    }
    /* XXX */
    TRY_STAGE(vert, GL_VERTEX_SHADER)
    TRY_STAGE(frag, GL_FRAGMENT_SHADER)
    TRY_STAGE(tese, GL_TESS_EVALUATION_SHADER)
    TRY_STAGE(tesc, GL_TESS_CONTROL_SHADER)
    TRY_STAGE(geom, GL_GEOMETRY_SHADER)
#ifndef FE_TARGET_EMSCRIPTEN
    TRY_STAGE(comp, GL_COMPUTE_SHADER) /* Check it anyway. It MUST cause an error. */
#endif
#undef TRY_STAGE

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
        /* fe_logv(TAG, "Successfully linked \"%s\".\n", save_path); */
#ifdef FE_GL_DBG
        glValidateProgram(program); // >= ES 2.0
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        fe_logv(TAG, "\"%s\" is %svalid. ", 
                        save_path, status==GL_TRUE ? "" : "in");
        fe_gl_log_program_info(program, fe_logv_not_macro);
        fe_logv(TAG, "\n");
#endif
        return true;
    }

    fe_loge(TAG, "Could not link \"%s\" :\n\t", "program");
    fe_gl_log_program_info(program, fe_loge);
    fe_loge(TAG, "\n");
    return false;
}

static bool fe_gl_mkprog_4_1(GLuint program, 
                             fe_timestamp last_build_time,
                             fe_iov *restrict progbin,
                             const fe_gl_shader_source_set *restrict ss) {

    if(progbin) {
        FE_COMPILETIME_ASSERT(sizeof(fe_timestamp)==8, "Expected to swap 8 bytes.");
        fe_timestamp progbin_ts = fe_hw_swap64_net_to_host(*(fe_timestamp*)progbin->base);
        bool outdated = (progbin_ts <= last_build_time);

        if(!outdated)
            if(fgm_program_from_binary(program, progbin))
                return true;
    }

#ifndef FE_TARGET_EMSCRIPTEN
    glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, 
                        GL_TRUE);
#endif
    if(!fe_gl_mkprog_2_0(program, last_build_time, progbin, ss))
        return false;
    if(progbin)
        fgm_program_to_binary(program, progbin);
    return true;
}

bool (*fe_gl_mkprog)(GLuint progid, fe_timestamp last_build_time, fe_iov *restrict progbin,
                     const fe_gl_shader_source_set *restrict ss);


void fe_gl_mkprog_setup(const fe_gl_version *v) {
    unsigned version = v->major*10 + v->minor;

    if(version < 20) {
        fe_logw(TAG, "fe_gl_mkprog is not available, because the OpenGL "
                     "version is less than 2.0.\n"
                     "If you see this, something is wrong with "
                     "the caller's code.\n"); 
        return;
    }

    for(shader_types_db = shader_types_db_actual ; ; ++shader_types_db)
        if(shader_types_db->min_gl_version <= version)
            break;
    if(v->es) {
        fe_gl_mkprog = (version>=30 ? fe_gl_mkprog_4_1 : fe_gl_mkprog_2_0);
        fe_gl_mkprog_cleanup = fe_gl_mkprog_cleanup_4_1;
    } else {
        if(version >= 41) {
            fe_gl_mkprog = fe_gl_mkprog_4_1;
            fe_gl_mkprog_cleanup = fe_gl_mkprog_cleanup_4_1;
        } else {
            fe_gl_mkprog = fe_gl_mkprog_2_0;
            fe_gl_mkprog_cleanup = fe_gl_mkprog_cleanup_2_0;
        }
    }
}
