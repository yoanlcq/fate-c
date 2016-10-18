#include <fate/fate.h>
#include "cubemap.h"
#include "res.h"

typedef union {
    struct {uint32_t r:8,g:8,b:8,a:8;};
    uint8_t ary[4];
} fe_rgba32;

typedef union {
    struct {uint32_t r:8,g:8,b:8;};
    uint8_t ary[3];
} fe_rgb24;

typedef struct {
    size_t w, h;
    fe_rgb24 *pixels;
} fe_rgb24_img;

bool fe_rgb24_img_init_auto(fe_rgb24_img *img, const fe_iov *iov) {
    int w, h, component_count;
    uint8_t *bytes = stbi_load_from_memory(iov->base, iov->len, &w, &h, &component_count, 3);
    // fe_dbg_hope(component_count == 3); // doc says we can count on it.
    img->w = w;
    img->h = h;
    img->pixels = (void*)bytes;
    return !!img->pixels;
}
void fe_rgb24_img_deinit(fe_rgb24_img *img) {
    stbi_image_free(img->pixels);
}

static void res_load_rgb24_img(fe_rgb24_img *img, const char *filepath) {
    fe_iov iov;
    fe_dbg_hope(res_load(&iov, filepath));
    fe_dbg_hope(fe_rgb24_img_init_auto(img, &iov));
    fe_iov_deinit(&iov);
}

GLuint cubemap_build_grouse(void) {
    GLuint cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    const GLint lod = 0;
    fe_rgb24_img img;
#define HELPER(sign,axis,face) \
    res_load_rgb24_img(&img, res.cubemaps.flame.face); \
    glTexImage2D(GL_TEXTURE_CUBE_MAP_##sign##_##axis, lod, GL_RGB, \
                 img.w, img.h, 0, GL_RGB, GL_UNSIGNED_BYTE, img.pixels); \
    fe_rgb24_img_deinit(&img)
    HELPER(POSITIVE,X,ft);
    HELPER(NEGATIVE,Z,lf); 
    HELPER(NEGATIVE,X,bk); 
    HELPER(POSITIVE,Z,rt); 
    HELPER(POSITIVE,Y,up);
    HELPER(NEGATIVE,Y,dn);
#undef HELPER
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return cubemap;
}

GLuint cubemap_build_6cols(void) {
    GLuint cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    const GLint lod = 0;
    const GLsizei w = 1, h = 1;
    GLubyte data[6][1*1][4] = {
        { {255,   0,   0, 255} },
        { {255, 255,   0, 255} },
        { {  0, 255,   0, 255} },
        { {  0, 255, 255, 255} },
        { {  0,   0, 255, 255} },
        { {255,   0, 255, 255} }
    };
#define HELPER(sign,axis,data) \
    glTexImage2D(GL_TEXTURE_CUBE_MAP_##sign##_##axis, lod, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data)
    HELPER(POSITIVE,X,data[0]);
    HELPER(NEGATIVE,X,data[1]);
    HELPER(POSITIVE,Y,data[2]);
    HELPER(NEGATIVE,Y,data[3]);
    HELPER(POSITIVE,Z,data[4]);
    HELPER(NEGATIVE,Z,data[5]);
#undef HELPER
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return cubemap;
}
