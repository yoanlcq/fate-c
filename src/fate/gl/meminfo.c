#include <fate/gl/defs.h>
#include <fate/gl/utils.h>
#include <fate/gl/meminfo.h>

static void fgl_meminfo_query_dummy(fate_gl_meminfo *m) {
    m->type = FATE_GL_MEMINFO_NONE;
}
static void fgl_meminfo_query_nvx(fate_gl_meminfo *m) {
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

