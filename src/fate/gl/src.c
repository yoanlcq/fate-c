#include <fate/fate.h>

static const char *TAG = "fe_gl_src";

static fe_timestamp timestamp = 0;

fe_timestamp fe_gl_src_get_build_timestamp(void) {
    return timestamp ? timestamp : (timestamp = fe_timestamp_here());
}

void fe_gl_src_cube_prelink(const fe_gl_shader_source_set *ss, GLuint prog, fe_timestamp last_build_time) {
	glBindAttribLocation(prog, 0, "a_pos");
	//glBindAttribLocation(prog, 1, "a_norm");
	//glBindAttribLocation(prog, 2, "a_texcoord");
	//glBindAttribLocation(prog, 3, "a_col");
}

void fe_gl_src_cube_locs_init(fe_gl_src_cube_locs *locs, const fe_gl_src_config *cfg, GLuint prog) {

    bool all_uniforms_located = true;

#define HELPER(var) \
    locs->var = glGetUniformLocation(prog, "u_"#var); \
    if(locs->var == -1) { \
        fe_loge(TAG, "glGetUniformLocation(\"%s\") returned -1 !\n", "u_"#var);\
        all_uniforms_located = false; \
    }

    HELPER(mvp_matrix);
    HELPER(mv_matrix);
    HELPER(normal_matrix);
    HELPER(lights_datatex);
    HELPER(mtls_datatex);
    HELPER(material_for_front);
    HELPER(material_for_back);
    HELPER(cubemap_for_front);
    HELPER(cubemap_for_back);
    HELPER(ambient_global);
    HELPER(eye_direction);
    HELPER(skybox);
#undef HELPER
    fe_dbg_hope(all_uniforms_located);
}

void fe_gl_src_cube_locs_deinit(fe_gl_src_cube_locs *locs) {}

void fe_gl_src_get_cube_vert(fe_iov *iov, const fe_gl_src_config *cfg) {

    unsigned version = cfg->es ? 100 : 130;
    const char *attr = cfg->es ? "attribute" : "in";
    const char *varying = cfg->es ? "varying" : "out";
    fe_iov_printf(iov, iov->len, 
        "// cube.%u%s.vert\n"
        "#version %u\n"
        "\n"
        "#pragma optimize(%s)\n"
        "#pragma debug(%s)\n"
        "\n"
        "%s vec4 a_pos;\n"
        "%s vec3 v_norm;\n"
        "%s vec4 v_pos;\n"
        "uniform mat4 u_mvp_matrix;\n"
        "uniform mat4 u_mv_matrix;\n"
        "uniform mat4 u_normal_matrix;\n"
        "\n"
        "void main() {\n"
        "    v_norm = normalize(u_normal_matrix * a_pos).xyz;\n"
        "    v_pos = u_mv_matrix * a_pos;\n"
        "    gl_Position = u_mvp_matrix * a_pos;\n"
        "}\n"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off", 
        attr, varying, varying
    );
}

void fe_gl_src_get_cube_frag(fe_iov *iov, const fe_gl_src_config *cfg) {

    unsigned version = cfg->es ? 100 : 130;
    const char *varying = cfg->es ? "varying" : "in";
    fe_iov_printf(iov, iov->len, 
        "// cube.%u%s.frag\n"
        "#version %u\n"
        "\n"
        "#pragma optimize(%s)\n"
        "#pragma debug(%s)\n"
        "\n"
        "#ifdef GL_ES\n"
        "    #ifdef GL_FRAGMENT_PRECISION_HIGH\n"
        "        precision highp float;\n"
        "    #else\n"
        "        precision mediump float;\n"
        "    #endif\n"
        "#endif\n"
        "\n"
        "uniform sampler2D u_lights_datatex;\n"
        "uniform sampler2D u_mtls_datatex;\n"
        "uniform int u_material_for_front;\n"
        "uniform int u_material_for_back;\n"
        "uniform samplerCube u_cubemap_for_front;\n"
        "uniform samplerCube u_cubemap_for_back;\n"
        "uniform vec4 u_ambient_global;\n"
        "uniform vec3 u_eye_direction;\n"
        "uniform samplerCube u_skybox;\n"
        "\n"
        "%s vec3 v_norm;\n"
        "%s vec4 v_pos;\n"
        "\n"
        "#ifdef GL_ES\n"
            "#define f_col gl_FragColor\n"
        "#else\n"
            "out vec4 f_col;\n"
        "#endif //GL_ES\n"
        "\n"
        "#define LIGHT_MAX %u\n"
        "#define MTL_MAX %u\n"
"#define light_setup(ignored) "
        "vec4 light_v0; "
        "vec4 light_v1; "
        "vec4 light_v2; "
        "vec4 light_v3; "
        "vec4 light_v4; "
        "vec4 light_v5; "
        "vec4 light_v6; "
        "\n"
"#define light_use(l) "
        "light_v0 = texture2D(u_lights_datatex, vec2(0.0, float(l))); "
        "light_v1 = texture2D(u_lights_datatex, vec2(1.0, float(l))); "
        "light_v2 = texture2D(u_lights_datatex, vec2(2.0, float(l))); "
        "light_v3 = texture2D(u_lights_datatex, vec2(3.0, float(l))); "
        "light_v4 = texture2D(u_lights_datatex, vec2(4.0, float(l))); "
        "light_v5 = texture2D(u_lights_datatex, vec2(5.0, float(l))); "
        "light_v6 = texture2D(u_lights_datatex, vec2(6.0, float(l))); "
        "\n"
"#define light_is_enabled            (bool(light_v0.x))\n"
"#define light_is_local              (bool(light_v0.y))\n"
"#define light_is_spot               (bool(light_v0.z))\n"
"#define light_spot_cos_cutoff       (light_v0.w)\n"
"#define light_ambient               (light_v1.xyz)\n"
"#define light_color                 (light_v2.xyz)\n"
"#define light_position              (light_v3.xyz)\n"
"#define light_half_vector           (light_v4.xyz)\n"
"#define light_cone_direction        (light_v5.xyz)\n"
"#define light_spot_exponent         (light_v6.x)\n"
"#define light_constant_attenuation  (light_v6.y)\n"
"#define light_linear_attenuation    (light_v6.z)\n"
"#define light_quadratic_attenuation (light_v6.w)\n"
"#define mtl_setup(ignored) "
        "vec4 mtl_v0; "
        "vec4 mtl_v1; "
        "vec4 mtl_v2; "
        "vec4 mtl_v3; "
        "vec4 mtl_v4; "
        "\n"
"#define mtl_use(m) "
        "mtl_v0 = texture2D(u_mtls_datatex, vec2(0.0, float(m))); "
        "mtl_v1 = texture2D(u_mtls_datatex, vec2(1.0, float(m))); "
        "mtl_v2 = texture2D(u_mtls_datatex, vec2(2.0, float(m))); "
        "mtl_v3 = texture2D(u_mtls_datatex, vec2(3.0, float(m))); "
        "mtl_v4 = texture2D(u_mtls_datatex, vec2(4.0, float(m))); "
        "\n"
"#define mtl_is_affected_by_lights (bool(mtl_v0.x))\n"
"#define mtl_shininess             (mtl_v0.y)\n"
"#define mtl_strength              (mtl_v0.z)\n"
"#define mtl_emission              (mtl_v1.xyz)\n"
"#define mtl_ambient               (mtl_v2.xyz)\n"
"#define mtl_diffuse               (mtl_v3.xyz)\n"
"#define mtl_specular              (mtl_v4.xyz)\n"
        "\n"
        "void main() {\n"
        "    mtl_setup(0);\n"
        "    light_setup(0);\n"
        "    int m = (gl_FrontFacing ? u_material_for_front : u_material_for_back);\n"
        "    mtl_use(m);\n"
        "    vec3 scattered_light = u_ambient_global.xyz;\n"
        "    vec3 reflected_light = vec3(0.0);\n"
        "    for(int l=0 ; l<LIGHT_MAX ; ++l) {\n"
        "        light_use(l);\n"
        "        if(!light_is_enabled\n"
        "        || !mtl_is_affected_by_lights)\n"
        "            continue;\n"
        "        vec3 half_vector;\n"
        "        vec3 light_direction = light_position;\n"
        "        float attenuation = 1.0;\n"
        "        if(light_is_local) {\n"
        "            light_direction -= vec3(v_pos);\n"
        "            float light_distance = length(light_direction);\n"
        "            light_direction /= light_distance; \n"
        "            attenuation = 1.0 / (\n"
        "                light_constant_attenuation\n"
        "              + light_linear_attenuation    * light_distance\n"
        "              + light_quadratic_attenuation * light_distance\n"
        "                                            * light_distance\n"
        "            );\n"
        "            if(light_is_spot) {\n"
        "                float spot_cos = dot(light_direction, -light_cone_direction);\n"
        "                if(spot_cos < light_spot_cos_cutoff)\n"
        "                    attenuation = 0.0;\n"
        "                else\n"
        "                    attenuation *= pow(spot_cos, light_spot_exponent);\n"
        "            }\n"
        "            half_vector = normalize(light_direction + u_eye_direction);\n"
        "        } else {\n"
        "            half_vector = light_half_vector;\n"
        "        }\n"
        "        float diffuse = max(0.0, dot(v_norm, light_direction));\n"
        "        float specular = max(0.0, dot(v_norm, half_vector));\n"
        "        if(diffuse == 0.0)\n"
        "            specular = 0.0;\n"
        "        else\n"
        "            specular = pow(specular, mtl_shininess)\n"
        "                     * mtl_strength;\n"
        "\n"
        "        scattered_light += light_ambient*attenuation\n"
        "                        +  light_color*attenuation*diffuse;\n"
        "        reflected_light += light_color*attenuation*specular;\n"
        "    } //end for each enabled light\n"
        "\n"
        "    vec4 texel;\n"
        "    if(m==u_material_for_front)\n"
        "       texel = textureCube(u_cubemap_for_front, v_norm);\n"
        "    else\n"
        "       texel = textureCube(u_cubemap_for_back, v_norm);\n"
        "    vec4 sky_texel = textureCube(u_skybox, v_norm); //HACK to use u_skybox. Does not use rotation.\n"
        "    sky_texel *= 0.000001;"
        "    vec3 rgb = min((texel.rgb+sky_texel.rgb)*scattered_light + reflected_light, vec3(1.0));\n"
        "    f_col = vec4(rgb, texel.a);\n"
        "}\n"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off",
        varying, varying,
        cfg->light_count, cfg->material_count
    );
}
