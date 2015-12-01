#include <string.h>
#include <fate/gl/defs.h>
#include <fate/gl/debug.h>
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
    GLubyte indices[17] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        0xFF,
        5, 3, 7, 1, 6, 0, 4, 2
    };

    c->prog = prog;
    glUseProgram(c->prog);

    glGenBuffers(1, &c->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    fate_glObjectLabel(GL_BUFFER, c->ebo, strlen("\"Cube EBO\""), "\"Cube EBO\"");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glGenVertexArrays(1, &c->vao);
    glBindVertexArray(c->vao);
 
    glGenBuffers(1, &c->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    fate_glObjectLabel(GL_BUFFER, c->vbo, strlen("\"Cube VBO\""), "\"Cube VBO\"");
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
    glBindVertexArray(c->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    glPrimitiveRestartIndex(0xFF);
    glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_BYTE, NULL);
}

