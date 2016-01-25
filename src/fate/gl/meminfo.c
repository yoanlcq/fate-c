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

/*! \file fate/gl/meminfo.c
 *  \brief OpenGL extensions wrapper for getting GPU memory info.
 *
 * TODO
 */

#include <fate/gl/defs.h>
#include <fate/gl/meminfo.h>

static void fgl_meminfo_query_dummy(fate_gl_meminfo *m) {
    m->type = FATE_GL_MEMINFO_NONE;
}
static void fgl_meminfo_query_nvx(fate_gl_meminfo *m) {
#ifndef FATE_EMSCRIPTEN
    m->type = FATE_GL_MEMINFO_NVX;
    glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, 
                  &(m->data.nvx.dedicated_vidmem));
    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,
                  &(m->data.nvx.total_available_mem));
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,
                  &(m->data.nvx.current_available_vidmem));
    glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX,
                  &(m->data.nvx.eviction_count));
    glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX,
                  &(m->data.nvx.evicted_mem));
#endif
}
static void fgl_meminfo_query_ati(fate_gl_meminfo *m) {
    GLint buf[4];

    m->type = FATE_GL_MEMINFO_ATI;
#define HELPER(TYPE, type) \
    glGetIntegerv(GL_##TYPE##_FREE_MEMORY_ATI, buf); \
    m->data.ati.type##_free_memory.total             = buf[0]; \
    m->data.ati.type##_free_memory.largest_block     = buf[1]; \
    m->data.ati.type##_free_memory.total_aux         = buf[2]; \
    m->data.ati.type##_free_memory.largest_aux_block = buf[3];
    HELPER(VBO, vbo);
    HELPER(TEXTURE, texture);
    HELPER(RENDERBUFFER, renderbuffer);
#undef HELPER
}

void (*fate_gl_meminfo_query)(fate_gl_meminfo *m);

void fate_gl_meminfo_setup(GLint gl_major, GLint gl_minor) {
    if(GLEW_NVX_gpu_memory_info)
        fate_gl_meminfo_query = fgl_meminfo_query_nvx;
    else if(GLEW_ATI_meminfo)
        fate_gl_meminfo_query = fgl_meminfo_query_ati;
    else fate_gl_meminfo_query = fgl_meminfo_query_dummy;
}

