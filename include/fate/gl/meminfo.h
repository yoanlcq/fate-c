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

/*! \file fate/gl/meminfo.h
 *  \brief OpenGL extensions wrapper for getting GPU memory info.
 *
 *  TODO
 */

#ifndef FATE_GL_MEMINFO_H
#define FATE_GL_MEMINFO_H

#include <fate/gl/defs.h>

struct fate_gl_meminfo_nvx {
    GLint dedicated_vidmem;
    GLint total_available_mem;
    GLint current_available_vidmem;
    GLint eviction_count;
    GLint evicted_mem;
};

struct fate_gl_meminfo_ati_field {
    GLint total;
    GLint largest_block;
    GLint total_aux;
    GLint largest_aux_block;
};

struct fate_gl_meminfo_ati {
    struct fate_gl_meminfo_ati_field vbo_free_memory;
    struct fate_gl_meminfo_ati_field texture_free_memory;
    struct fate_gl_meminfo_ati_field renderbuffer_free_memory;
};

union fate_gl_meminfo_data {
    struct fate_gl_meminfo_nvx nvx;
    struct fate_gl_meminfo_ati ati;
};

enum fate_gl_meminfo_type {
    FATE_GL_MEMINFO_NONE = 0,
    FATE_GL_MEMINFO_NVX = 1,
    FATE_GL_MEMINFO_ATI = 2
};

struct fate_gl_meminfo {
    enum  fate_gl_meminfo_type type;
    union fate_gl_meminfo_data data;
};
typedef struct fate_gl_meminfo fate_gl_meminfo;

void (*fate_gl_meminfo_query)(fate_gl_meminfo *m);
void fate_gl_meminfo_setup(GLint gl_major, GLint gl_minor);

#endif /* FATE_GL_MEMINFO_H */
