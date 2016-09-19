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
static fe_gl_src_config scfg = {0};
static fe_gl_src_cube_locs locs = {0};

static GLuint cubemap_6cols = 0;
static GLuint cubemap_grouse = 0;
static GLuint datatex_lights = 0;
static GLuint datatex_mtls = 0;

static GLuint datatex_build_mtls(void) {
    GLuint tex = 0;
    size_t i;
    fe_gl_src_mtl_datatex_line *lines = fe_mem_heapalloc(
        scfg.material_count, fe_gl_src_mtl_datatex_line, ""
    );
    fe_dbg_hope(lines);
    for(i=0 ; i<scfg.material_count ; ++i) { 
        mdatatex__is_affected_by_lights(lines[i]) = false; /*(i>0); */
        mdatatex__shininess(lines[i]) = 128;
        mdatatex__strength(lines[i]) = 128;
        u8vec4 zeroes = {0, 0, 0, 0};
        u8vec4 ones = {255, 255, 255, 255};
        memcpy(mdatatex__emission(lines[i]), zeroes, sizeof(u8vec4));
        memcpy(mdatatex__ambient(lines[i]) , ones,   sizeof(u8vec4));
        memcpy(mdatatex__diffuse(lines[i]) , ones,   sizeof(u8vec4));
        memcpy(mdatatex__specular(lines[i]), ones,   sizeof(u8vec4));
    }
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                 sizeof(*lines)/sizeof(u8vec4), scfg.material_count, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, lines);
    fe_mem_heapfree(lines);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(  GL_TEXTURE_2D, 0);
    return tex;
}

static GLuint datatex_build_lights(void) {
    GLuint tex = 0;
    size_t i;
    fe_gl_src_light_datatex_line *lines = fe_mem_heapalloc(
        scfg.light_count, fe_gl_src_light_datatex_line, ""
    );
    fe_dbg_hope(lines);

    for(i=0 ; i<scfg.light_count ; ++i) {
        ldatatex__is_enabled(           lines[i]) = true;
        ldatatex__is_local(             lines[i]) = true;
        ldatatex__is_spot(              lines[i]) = false;
        u8vec4 ones  = {255, 255, 255, 255};
        u8vec4 red   = {255, 0, 0, 255};
        u8vec4 twos  = {200, 200, 200, 255};
        u8vec4 fives = {255, 255, 255, 255};
        memcpy(ldatatex__ambient(              lines[i]),  ones, sizeof(u8vec4));
        memcpy(ldatatex__color(                lines[i]),   red, sizeof(u8vec4));
        memcpy(ldatatex__position(             lines[i]),  twos, sizeof(u8vec4));
        memcpy(ldatatex__half_vector(          lines[i]), fives, sizeof(u8vec4));
        memcpy(ldatatex__cone_direction(       lines[i]), fives, sizeof(u8vec4));
        ldatatex__spot_cos_cutoff(      lines[i]) = 128;
        ldatatex__spot_exponent(        lines[i]) = 255;
        ldatatex__constant_attenuation( lines[i]) = 200;
        ldatatex__linear_attenuation(   lines[i]) = 0;
        ldatatex__quadratic_attenuation(lines[i]) = 0;
    }
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                 sizeof(*lines)/sizeof(u8vec4), scfg.material_count, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, lines);
    fe_mem_heapfree(lines);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(  GL_TEXTURE_2D, 0);
    return tex;
}

static void prepare_texs(void) {
    cubemap_grouse = cubemap_build_grouse();
    cubemap_6cols = cubemap_build_6cols();
    datatex_lights = datatex_build_lights();
    datatex_mtls = datatex_build_mtls();
    glUseProgram(prog); 
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_grouse);
    glUniform1i(locs.skybox, 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_6cols);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, datatex_lights);
    glUniform1i(locs.lights_datatex, 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, datatex_mtls);
    glUniform1i(locs.mtls_datatex, 4);
    glActiveTexture(GL_TEXTURE0);
}

void Cube_setup(void) {
    prog = glCreateProgram();
    fe_gl_dbg_glObjectLabel(GL_PROGRAM, prog, -1, "\"Cube program\"");
#ifdef FE_GL_TARGET_ES
    scfg.es = true;
#endif
    scfg.debug = true;
    scfg.optimize = true;
    scfg.light_count = sizeof(fe_gl_src_light_datatex_line)/sizeof(u8vec4); /* To have a POT texture. */
    scfg.material_count = sizeof(fe_gl_src_mtl_datatex_line)/sizeof(u8vec4); /* Idem. */
    fe_gl_shader_source_set ss = {{0}};
    fe_gl_src_get_cube_vert(&ss.vert, &scfg);
    fe_gl_src_get_cube_frag(&ss.frag, &scfg);
    ss.before_linking = fe_gl_src_cube_prelink;
    if(!fe_gl_mkprog_no_binary(prog, &ss))
        fe_fatal(TAG, "Could not build the OpenGL program!\n"
		"More details on this error have been logged.\n");
    fe_iov_deinit(&ss.vert);
    fe_iov_deinit(&ss.frag);

    glUseProgram(prog); 
    fe_gl_src_cube_locs_init(&locs, &scfg, prog);
    prepare_texs();
    glUseProgram(0);

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
    glDeleteTextures(1, &datatex_lights);
    glDeleteTextures(1, &datatex_mtls);
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
        glUniform1i (locs.material_for_front, cubes[i]->material_for_front);
        glUniform1i (locs.material_for_back,  cubes[i]->material_for_back);
        glUniform1i (locs.cubemap_for_front,  cubes[i]->material_for_front+1);
        glUniform1i (locs.cubemap_for_back,   cubes[i]->material_for_back+1);
        glUniform4fv(locs.ambient_global, 1, cubes[i]->ambient);
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


