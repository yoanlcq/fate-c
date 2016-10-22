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
    fe_rgba32 *pixels;
} fe_rgba32_img;

bool fe_rgba32_img_init_auto(fe_rgba32_img *img, const fe_iov *iov) {
    int w, h, component_count;
    uint8_t *bytes = stbi_load_from_memory((void*)iov->base, iov->len, &w, &h, &component_count, 4);
    // fe_dbg_hope(component_count == 4); // doc says we can count on it.
    img->w = w;
    img->h = h;
    img->pixels = (void*)bytes;
    /*
    fe_logw("foo", "Iov->len : %"PRIsize_t"\n", iov->len);
    char foo[5];
    memcpy(foo, iov->base+6, sizeof(foo)-1);
    foo[sizeof(foo)-1] = '\0';
    fe_logw("foo", "JPG file : %s\n", foo);
    */
    return !!img->pixels;
}
void fe_rgba32_img_deinit(fe_rgba32_img *img) {
#ifndef FE_TARGET_EMSCRIPTEN
    stbi_image_free(img->pixels);
#else
    /* XXX Free the SDL_Surface. */
#endif
}
#if defined(FE_TARGET_EMSCRIPTEN)
#include <SDL2/SDL_image.h>
#endif

static void res_load_rgba32_img(fe_rgba32_img *img, const char *filepath) {
#if defined(FE_TARGET_EMSCRIPTEN)
    /*
     * Emscripten doc says that if we use --use-preload-plugins, our calls to IMG_Init will fail,
     * even though we're able to load common image types via IMG_Load.
    static bool wasinit = false;
    if(!wasinit) {
        int flags=IMG_INIT_JPG;
        int initted=IMG_Init(flags);
        if((initted&flags) != flags) {
            fe_logc("foo", "IMG_Init: Failed to init required jpg and png support!\n");
            fe_logc("foo", "IMG_Init: %s\n", IMG_GetError());
        }
        wasinit = true;
    }
    */
    fe_fpath fpath = res_path(filepath);
    fe_dbg_hope(fpath.is_memfs);
    fe_logi(TAG, "Loading img `%s'...\n", fpath.memfs.path);
    SDL_Surface *s = IMG_Load(fpath.memfs.path);
    fe_fpath_deinit(fpath);
    fe_dbg_hope(s && "Surface initialized");
    // Well it's expected to be 3, but IMG_Load doesn't seem to agree.
    // Hand-converting it to RGB actually has a crazy negative impact
    // on the runtime : Going from near-instantaneous startup time to
    // nearly a minute.
    // So forget it, just live with the fact it's RGBA.
    fe_dbg_hope(s->format->BytesPerPixel == 4);
    img->pixels = s->pixels;
    img->w = s->w;
    img->h = s->h;
#else
    fe_iov iov = {0};
    fe_dbg_hope(res_load(&iov, filepath));
    fe_dbg_hope(iov.base && iov.len);
    fe_dbg_hope(fe_rgba32_img_init_auto(img, &iov));
    fe_iov_deinit(&iov);
#endif
}

GLuint cubemap_build_grouse(void) {
    GLuint cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    const GLint lod = 0;
    fe_rgba32_img img;
#define HELPER(sign,axis,face) \
    res_load_rgba32_img(&img, res.cubemaps.flame.face.jpg); \
    glTexImage2D(GL_TEXTURE_CUBE_MAP_##sign##_##axis, lod, GL_RGBA, \
                 img.w, img.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.pixels); \
    fe_rgba32_img_deinit(&img)
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
