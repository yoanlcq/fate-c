#ifndef FE_GL_SRC
#define FE_GL_SRC

#include <fate/decl.h>
#include <fate/gl.h>
#include <fate/timestamp.h>
#include <fate/iov.h>
#include <linmath/linmath.h>

fe_timestamp fe_gl_src_get_build_timestamp(void);

typedef struct {
    bool es    : 1;
    bool debug : 1;
    bool optimize : 1;
    size_t light_count;
    size_t material_count;
} fe_gl_src_config;

typedef struct {
    GLint mvp_matrix;
    GLint mv_matrix;
    GLint normal_matrix;
    GLint lights_datatex;
    GLint mtls_datatex;
    GLint material_for_front;
    GLint material_for_back;
    GLint cubemap_for_front;
    GLint cubemap_for_back;
    GLint ambient_global;
    GLint eye_direction;
    GLint skybox;
} fe_gl_src_cube_locs;

typedef GLubyte u8vec4[4];

typedef struct {
    u8vec4 v[8];
} fe_gl_src_light_datatex_line;

typedef struct {
    u8vec4 v[8];
} fe_gl_src_mtl_datatex_line;

#define ldatatex__is_enabled(l)            ((l).v[0][0])
#define ldatatex__is_local(l)              ((l).v[0][1])
#define ldatatex__is_spot(l)               ((l).v[0][2])
#define ldatatex__spot_cos_cutoff(l)       ((l).v[0][3])
#define ldatatex__ambient(l)               ((l).v[1])
#define ldatatex__color(l)                 ((l).v[2])
#define ldatatex__position(l)              ((l).v[3])
#define ldatatex__half_vector(l)           ((l).v[4])
#define ldatatex__cone_direction(l)        ((l).v[5])
#define ldatatex__spot_exponent(l)         ((l).v[6][0])
#define ldatatex__constant_attenuation(l)  ((l).v[6][1])
#define ldatatex__linear_attenuation(l)    ((l).v[6][2])
#define ldatatex__quadratic_attenuation(l) ((l).v[6][3])

#define mdatatex__is_affected_by_lights(m) ((m).v[0][0])
#define mdatatex__shininess(m)             ((m).v[0][1])
#define mdatatex__strength(m)              ((m).v[0][2])
#define mdatatex__emission(m)              ((m).v[1])
#define mdatatex__ambient(m)               ((m).v[2])
#define mdatatex__diffuse(m)               ((m).v[3])
#define mdatatex__specular(m)              ((m).v[4])

void fe_gl_src_cube_prelink(const fe_gl_shader_source_set *ss, GLuint prog, fe_timestamp last_build_time);
void fe_gl_src_get_cube_vert(fe_iov *iov, const fe_gl_src_config *cfg);
void fe_gl_src_get_cube_frag(fe_iov *iov, const fe_gl_src_config *cfg);
void fe_gl_src_cube_locs_init(fe_gl_src_cube_locs *locs, const fe_gl_src_config *cfg, GLuint prog);
void fe_gl_src_cube_locs_deinit(fe_gl_src_cube_locs *locs);


#endif /* FE_GL_SRC */
