#ifndef CUBE_H
#define CUBE_H

#include <fate/gl.h>

struct Cube {
    GLuint ebo, vao, vbo, prog;
};
typedef struct Cube Cube;

void Cube_init(Cube *c, GLuint prog);
void Cube_free(Cube *c);
void Cube_draw(Cube *c);

#endif/*CUBE_H*/
