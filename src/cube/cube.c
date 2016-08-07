#include <string.h>
#include <fate/defs.h>
#include <fate/gl.h>
#include "cube.h"

void Cube_init(Cube *c, GLuint prog) {
    GLbyte vertices[24] = { 
         127, -127, -127,
         127,  127, -127,
        -127, -127, -127,
        -127,  127, -127,
        -127, -127,  127,
        -127,  127,  127,
         127, -127,  127,
         127,  127,  127
    };
    GLubyte colors[24] = {
        255, 0, 0,
        255, 0, 0,
        0, 255, 0,
        0, 255, 0,
        0, 0, 255,
        0, 0, 255,
        255, 255, 0,
        255, 255, 0
    };
    GLubyte indices[16] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        5, 3, 7, 1, 6, 0, 4, 2
    };

    c->prog = prog;
    glUseProgram(c->prog);

    glGenBuffers(1, &c->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    fe_gl_dbg_glObjectLabel(GL_BUFFER, c->ebo, -1, "\"Cube EBO\"");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glGenVertexArrays(1, &c->vao);
    glBindVertexArray(c->vao);
    fe_gl_dbg_glObjectLabel(GL_VERTEX_ARRAY, c->vao, -1, "\"Cube VAO\"");
 
    glGenBuffers(1, &c->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    fe_gl_dbg_glObjectLabel(GL_BUFFER, c->vbo, -1, "\"Cube VBO\"");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), NULL, 
            GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 
            BUFFER_OFFSET(sizeof(vertices)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

}
void Cube_free(Cube *c) {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &c->ebo);
    glDeleteBuffers(1, &c->vbo);
    glDeleteVertexArrays(1, &c->vao);
}
void Cube_draw(Cube *c) {
    //glDisable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE_STRIP);
    glCullFace(GL_FRONT);
    glUseProgram(c->prog);
    glBindVertexArray(c->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    const GLsizei counts[2] = {8, 8};
    const GLvoid *const offsets[2] = {(GLvoid*)0, (GLvoid*)8};
    glMultiDrawElements(GL_TRIANGLE_STRIP, counts, GL_UNSIGNED_BYTE, offsets, 2);
}

void draw_some_image(plane2d *p) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(p->prog);
    glBindVertexArray(p->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->ebo);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, NULL);
    glEnable(GL_DEPTH_TEST);
}
