#ifndef CUBE_H
#define CUBE_H

#include <fate/gl.h>
#include <linmath/linmath.h>

struct Cube {
    GLuint tex_unit;
    mat4 mvp;
    bool see_inside;
};
typedef struct Cube Cube;

void Cube_setup(void);
void Cube_cleanup(void);
void Cube_init(Cube *c);
void Cube_deinit(Cube *c);
void Cube_multidraw(const Cube * const* cubes, size_t count);

struct plane2d {
    GLuint ebo, vbo, prog;
};
typedef struct plane2d plane2d;

void draw_some_image(plane2d *p);


#endif/*CUBE_H*/
