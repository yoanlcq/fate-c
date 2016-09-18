#include <string.h>
#include <fate/fate.h>
#include <linmath/linmath.h>
#include "cube.h"
#include "cubemap.h"

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
static fe_gl_src_cube_locs locs;

static GLuint cubemap_6cols = 0;
static GLuint cubemap_grouse = 0;

static void locs_fill(void) {
    glUseProgram(prog); 
    size_t i;
    for(i=0 ; i<locs.light_count ; ++i) {
        glUniform1i(locs.lights[i].is_enabled, true);
        glUniform1i(locs.lights[i].is_local, true);
        glUniform1i(locs.lights[i].is_spot, false);
        glUniform3f(locs.lights[i].ambient, 1.0f, 1.0f, 1.0f);
        glUniform3f(locs.lights[i].color, 1.0f, 0.f, 0.f);
        glUniform3f(locs.lights[i].position, 2.0f, 2.0f, 2.0f);
        glUniform3f(locs.lights[i].half_vector, 5.0f, 5.0f, 5.0f);
        glUniform3f(locs.lights[i].cone_direction, 5.0f, 5.0f, 5.0f);
        glUniform1f(locs.lights[i].spot_cos_cutoff, 0.5f);
        glUniform1f(locs.lights[i].spot_exponent, 1.f);
        glUniform1f(locs.lights[i].constant_attenuation, .6f);
        glUniform1f(locs.lights[i].linear_attenuation, 0.f);
        glUniform1f(locs.lights[i].quadratic_attenuation, 0.f);
    }

    cubemap_grouse = cubemap_build_grouse();
    cubemap_6cols = cubemap_build_6cols();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_grouse);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_6cols);
    glActiveTexture(GL_TEXTURE0);

    for(i=0 ; i<locs.material_count ; ++i) {
        glUniform1i(locs.materials[i].is_affected_by_lights, i>0);
        glUniform1i(locs.materials[i].cubemap, i+1);
        glUniform3f(locs.materials[i].emission, 0.f, 0.f, 0.f);
        glUniform3f(locs.materials[i].ambient,  1.f, 1.f, 1.f);
        glUniform3f(locs.materials[i].diffuse,  1.f, 1.f, 1.f);
        glUniform3f(locs.materials[i].specular, 1.f, 1.f, 1.f);
        glUniform1f(locs.materials[i].shininess, .5f);
        glUniform1f(locs.materials[i].strength, .5f);
    }
    glUseProgram(0);
}

void Cube_setup(void) {
    prog = glCreateProgram();
    fe_gl_dbg_glObjectLabel(GL_PROGRAM, prog, -1, "\"Cube program\"");
    fe_gl_src_config scfg = {0};
#ifdef FE_GL_TARGET_ES
    scfg.es = true;
#endif
    scfg.debug = true;
    scfg.optimize = true;
    scfg.light_count = 1;
    scfg.material_count = 4;
    fe_gl_shader_source_set ss = {{0}};
    fe_gl_src_get_cube_vert(&ss.vert, &scfg);
    fe_gl_src_get_cube_frag(&ss.frag, &scfg);
    ss.before_linking = fe_gl_src_cube_prelink;
    if(!fe_gl_mkprog_no_binary(prog, &ss))
        fe_fatal(TAG, "Could not build the OpenGL program!\n"
		"More details on this error have been logged.\n");

    fe_gl_src_cube_locs_init(&locs, &scfg, prog);
    locs_fill();

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
    fe_gl_src_cube_locs_deinit(&locs);
    glDeleteTextures(1, &cubemap_grouse);
    glDeleteTextures(1, &cubemap_6cols);
    glDeleteProgram(prog);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
}

void Cube_init(Cube *c) {
    memset(c, 0, sizeof(*c));
    mat4_identity(c->mvp_matrix);
    memcpy(c->mv_matrix, c->mvp_matrix, sizeof(mat4));
    memcpy(c->normal_matrix, c->mvp_matrix, sizeof(mat4));
}
void Cube_deinit(Cube *c) {}

void Cube_multidraw(const Cube * const *cubes, size_t count, const vec3 eye_direction) {

    glUseProgram(prog);

    static const GLsizei counts[2] = {8, 8};
    static const GLvoid *const offsets[2] = {(GLvoid*)0, (GLvoid*)8};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    size_t i;
    for(i=0 ; i<count ; ++i) {
        glUniformMatrix4fv(locs.mvp_matrix, 1, GL_FALSE, &cubes[i]->mvp_matrix[0][0]);
        glUniformMatrix4fv(locs.mv_matrix, 1, GL_FALSE, &cubes[i]->mv_matrix[0][0]);
        glUniformMatrix4fv(locs.normal_matrix, 1, GL_FALSE, &cubes[i]->normal_matrix[0][0]);
        glUniform4fv(locs.ambient_global, 1, cubes[i]->ambient);
        glUniform1i (locs.skybox, 1);
        glUniform1i (locs.material_for_front, cubes[i]->material_for_front);
        glUniform1i (locs.material_for_back, cubes[i]->material_for_back);
        glUniform3fv(locs.eye_direction, 1, eye_direction);

        switch(cubes[i]->cullface) {
        case CUBE_FACE_NONE:           glDisable(GL_CULL_FACE);       break;
        case CUBE_FACE_FRONT:          glCullFace(GL_FRONT);          break;  
        case CUBE_FACE_BACK:           glCullFace(GL_BACK);           break;  
        case CUBE_FACE_FRONT_AND_BACK: glCullFace(GL_FRONT_AND_BACK); break;  
        }

        glDrawElements(GL_TRIANGLE_STRIP, counts[0], GL_UNSIGNED_BYTE, offsets[0]);
        glDrawElements(GL_TRIANGLE_STRIP, counts[1], GL_UNSIGNED_BYTE, offsets[1]);

        if(!cubes[i]->cullface)
            glEnable(GL_CULL_FACE);
    }

    glDisableVertexAttribArray(0);
    glUseProgram(0);
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
