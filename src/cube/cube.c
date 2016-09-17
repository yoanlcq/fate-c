#include <string.h>
#include <fate/fate.h>
#include <linmath/linmath.h>
#include "cube.h"

static const char *TAG = "cube";

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
static const GLubyte indices[16] = {
    1, 0, 3, 2, 5, 4, 7, 6,
    4, 2, 6, 0, 7, 1, 5, 3
};


static GLuint prog=0, ebo=0, vbo=0;
static GLint mvp_loc=0, cubemap_loc=0;

void Cube_setup(void) {
    prog = glCreateProgram();
    fe_gl_dbg_glObjectLabel(GL_PROGRAM, prog, -1, "\"Cube program\"");
    fe_gl_src_config scfg = {0};
#ifdef FE_GL_TARGET_ES
    scfg.es = true;
#endif
    scfg.debug = true;
    scfg.optimize = true;
    fe_gl_shader_source_set ss = {{0}};
    fe_gl_src_get_cube_vert(&ss.vert, &scfg);
    fe_gl_src_get_cube_frag(&ss.frag, &scfg);
    ss.before_linking = fe_gl_src_cube_prelink;
    if(!fe_gl_mkprog_no_binary(prog, &ss))
        fe_fatal(TAG, "Could not build the OpenGL program!\n"
		"More details on this error have been logged.\n");

    mvp_loc = glGetUniformLocation(prog, "u_mvp");
    cubemap_loc= glGetUniformLocation(prog, "u_cubemap");

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    fe_gl_dbg_glObjectLabel(GL_BUFFER, ebo, -1, "\"Cube EBO\"");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    fe_gl_dbg_glObjectLabel(GL_BUFFER, vbo, -1, "\"Cube VBO\"");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

}

void Cube_cleanup(void) {
    glDeleteProgram(prog);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
}

void Cube_init(Cube *c) {
    memset(c, 0, sizeof(*c));
    mat4_identity(c->mvp);
}
void Cube_deinit(Cube *c) {}

void Cube_multidraw(const Cube * const *cubes, size_t count) {

    static const GLsizei counts[2] = {8, 8};
    static const GLvoid *const offsets[2] = {(GLvoid*)0, (GLvoid*)8};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glUseProgram(prog);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    size_t i;
    for(i=0 ; i<count ; ++i) {
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &cubes[i]->mvp[0][0]);
        glUniform1i(cubemap_loc, cubes[i]->tex_unit);
        glCullFace(cubes[i]->see_inside ? GL_FRONT : GL_BACK);

        glDrawElements(GL_TRIANGLE_STRIP, counts[0], GL_UNSIGNED_BYTE, offsets[0]);
        glDrawElements(GL_TRIANGLE_STRIP, counts[1], GL_UNSIGNED_BYTE, offsets[1]);
    }

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
