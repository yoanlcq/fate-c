#ifndef FE_GL_SRC
#define FE_GL_SRC

#include <fate/gl.h>
#include <fate/timestamp.h>
#include <fate/iov.h>

fe_timestamp fe_gl_src_get_build_timestamp(void);

typedef struct {
    bool es    : 1;
    bool debug : 1;
    bool optimize : 1;
    size_t light_count;
    size_t material_count;
} fe_gl_src_config;

typedef struct {
    GLint is_affected_by_lights;
    GLint cubemap;
    GLint emission;  // light produced by the material
    GLint ambient;   // what part of ambient light is reflected
    GLint diffuse;   // what part of diffuse light is scattered
    GLint specular;  // what part of specular light is scattered
    GLint shininess; // exponent for sharpening specular reflection
    GLint strength;
    //etc.
} fe_gl_materialpropslocs;

typedef struct {
    GLint is_enabled; // do we apply this light in this invocation ?
    GLint is_local; // true for a point light or spotlight, false for a positional light;
    GLint is_spot; // true for a spotlight
    GLint ambient; // light's contribution to ambient light
    GLint color; // color of light
    GLint position; // if is_local, location of light. Otherwise, direction toward the light.
    GLint half_vector; // direction of hightlights for directional light
    GLint cone_direction;
    GLint spot_cos_cutoff;
    GLint spot_exponent;
    GLint constant_attenuation; // local light attenuation coefficients
    GLint linear_attenuation;
    GLint quadratic_attenuation;
    //etc.
} fe_gl_lightpropslocs;

typedef struct {
    GLint mvp_matrix;
    GLint mv_matrix;
    GLint normal_matrix; // = transpose(inverse(modelView))
    GLint material_for_front;
    GLint material_for_back;
    GLint ambient_global;
    GLint eye_direction;
    GLint skybox;
    fe_gl_lightpropslocs *lights;
    size_t light_count; //convenience
    fe_gl_materialpropslocs *materials;
    size_t material_count; //convenience
} fe_gl_src_cube_locs;

void fe_gl_src_cube_prelink(const fe_gl_shader_source_set *ss, GLuint prog, fe_timestamp last_build_time);
void fe_gl_src_get_cube_vert(fe_iov *iov, const fe_gl_src_config *cfg);
void fe_gl_src_get_cube_frag(fe_iov *iov, const fe_gl_src_config *cfg);
void fe_gl_src_cube_locs_init(fe_gl_src_cube_locs *locs, const fe_gl_src_config *cfg, GLuint prog);
void fe_gl_src_cube_locs_deinit(fe_gl_src_cube_locs *locs);


#endif /* FE_GL_SRC */
