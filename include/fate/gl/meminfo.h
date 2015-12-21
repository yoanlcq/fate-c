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
