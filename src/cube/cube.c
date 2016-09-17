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
/*
 * Wrong winding
static const GLubyte indices[16] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    5, 3, 7, 1, 6, 0, 4, 2
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), NULL, 
            GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

}
void Cube_free(Cube *c) {
    glDeleteBuffers(1, &c->ebo);
    glDeleteBuffers(1, &c->vbo);
}
void Cube_draw(Cube *c) {
    glUseProgram(c->prog);

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 
            BUFFER_OFFSET(sizeof(vertices)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    const GLsizei counts[2] = {8, 8};
    const GLvoid *const offsets[2] = {(GLvoid*)0, (GLvoid*)8};
    glDrawElements(GL_TRIANGLE_STRIP, counts[0], GL_UNSIGNED_BYTE, offsets[0]);
    glDrawElements(GL_TRIANGLE_STRIP, counts[1], GL_UNSIGNED_BYTE, offsets[1]);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
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
