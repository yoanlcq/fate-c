#ifndef CUBE_H
#define CUBE_H

#include <opengl_base.h>
#include <linmath/linmath.h>

typedef enum Cube_input Cube_input;

typedef struct Cube_ {
    GLbyte  vertices[8][3];
    GLubyte colors[8][3];
    GLubyte indices[17];
    GLuint ebo, vao, vbo;
    mat4x4 model;
} Cube;

void Cube_init(Cube *c);
void Cube_free(Cube *c);
void Cube_draw(Cube *c);

#endif/*CUBE_H*/
