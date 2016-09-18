#ifndef CUBE_H
#define CUBE_H

#include <fate/gl.h>
#include <linmath/linmath.h>


typedef enum {
    CUBE_FACE_NONE = 0,
    CUBE_FACE_FRONT = 1,
    CUBE_FACE_BACK = 2,
    CUBE_FACE_FRONT_AND_BACK = 3,
} Cube_cullface;

struct Cube {
    mat4 mvp_matrix;
    mat4 mv_matrix;
    mat4 normal_matrix;
    Cube_cullface cullface : 2;
    size_t material_for_front;
    size_t material_for_back;
    vec4 ambient;
};
typedef struct Cube Cube;

void Cube_setup(void);
void Cube_cleanup(void);
void Cube_init(Cube *c);
void Cube_deinit(Cube *c);
void Cube_multidraw(const Cube * const *cubes, size_t count, const vec3 eye_direction);

struct plane2d {
    GLuint ebo, vbo, prog;
};
typedef struct plane2d plane2d;

void draw_some_image(plane2d *p);


#endif/*CUBE_H*/
