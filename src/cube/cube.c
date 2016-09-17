#include <string.h>
#include <fate/defs.h>
#include <fate/gl.h>
#include "cube.h"

static const GLbyte vertices[24] = { 
     127, -127, -127,
     127,  127, -127,
    -127, -127, -127,
    -127,  127, -127,
    -127, -127,  127,
    -127,  127,  127,
     127, -127,  127,
     127,  127,  127
};
/*
static const GLubyte colors[24] = {
    255, 0, 0,
    255, 0, 0,
    0, 255, 0,
    0, 255, 0,
    0, 0, 255,
    0, 0, 255,
    255, 255, 0,
    255, 255, 0
};
*/
static const GLubyte indices[16] = {
    1, 0, 3, 2, 5, 4, 7, 6,
    4, 2, 6, 0, 7, 1, 5, 3
};



void Cube_init(Cube *c, GLuint prog) {
    c->prog = prog;
    glUseProgram(c->prog);

    glGenBuffers(1, &c->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    fe_gl_dbg_glObjectLabel(GL_BUFFER, c->ebo, -1, "\"Cube EBO\"");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &c->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    fe_gl_dbg_glObjectLabel(GL_BUFFER, c->vbo, -1, "\"Cube VBO\"");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenTextures(1, &c->tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, c->tex);
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
}
void Cube_free(Cube *c) {
    glDeleteBuffers(1, &c->ebo);
    glDeleteBuffers(1, &c->vbo);
    glDeleteTextures(1, &c->tex);
}
void Cube_draw(Cube *c) {
    glUseProgram(c->prog);

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    const GLsizei counts[2] = {8, 8};
    const GLvoid *const offsets[2] = {(GLvoid*)0, (GLvoid*)8};
    glDrawElements(GL_TRIANGLE_STRIP, counts[0], GL_UNSIGNED_BYTE, offsets[0]);
    glDrawElements(GL_TRIANGLE_STRIP, counts[1], GL_UNSIGNED_BYTE, offsets[1]);

    glDisableVertexAttribArray(0);
}

void draw_some_image(plane2d *p) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(p->prog);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->ebo);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, NULL);
    glEnable(GL_DEPTH_TEST);
}
