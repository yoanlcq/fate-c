#ifndef CUBE_H
#define CUBE_H

#include <fate/gl.h>
#include <linmath/linmath.h>

struct Cube {
    GLuint ebo, vao, vbo;
    mat4x4 model;
} 
typedef struct Cube Cube;

void Cube_init(Cube *c);
void Cube_free(Cube *c);
void Cube_draw(Cube *c);

#endif/*CUBE_H*/
