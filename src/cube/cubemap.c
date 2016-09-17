#include <fate/gl.h>
#include "cubemap.h"

#include "grouse_bk.c.array"
#include "grouse_dn.c.array"
#include "grouse_ft.c.array"
#include "grouse_lf.c.array"
#include "grouse_rt.c.array"
#include "grouse_up.c.array"

GLuint cubemap_build_grouse(void) {
    GLuint cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    const GLint lod = 0;
#define HELPER(sign,axis,face) \
    glTexImage2D(GL_TEXTURE_CUBE_MAP_##sign##_##axis, lod, GL_RGBA, \
                 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, grouse_##face##_rgba)
    HELPER(POSITIVE,X,ft); //rt
    HELPER(NEGATIVE,Z,lf); //ft
    HELPER(NEGATIVE,X,bk); //lf
    HELPER(POSITIVE,Z,rt); //bk
    HELPER(POSITIVE,Y,up);
    HELPER(NEGATIVE,Y,dn);
#undef HELPER
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
