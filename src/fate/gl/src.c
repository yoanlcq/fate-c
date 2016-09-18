#include <fate/fate.h>

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

    locs->lights = fe_mem_heapalloc(cfg->light_count, fe_gl_lightpropslocs, "");
    fe_dbg_hope(locs->lights);
    locs->light_count = cfg->light_count;

    locs->materials = fe_mem_heapalloc(cfg->material_count, fe_gl_materialpropslocs, "");
    fe_dbg_hope(locs->materials);
    locs->material_count = cfg->material_count;

#define HELPER(var) locs->var = glGetUniformLocation(prog, "u_"#var)
    HELPER(mvp_matrix);
    HELPER(mv_matrix);
    HELPER(normal_matrix);
    HELPER(material_for_front);
    HELPER(material_for_back);
    HELPER(ambient_global);
    HELPER(eye_direction);
    HELPER(skybox);
#undef HELPER

    char buf[128];
    size_t i;
#define HELPER(ary, var) \
    sprintf(buf, "u_%ss[%"PRIsize_t"].%s", #ary, i, #var);\
    locs->ary##s[i].var = glGetUniformLocation(prog, buf)
    for(i=0 ; i<locs->light_count ; ++i) {
        HELPER(light,is_enabled);
        HELPER(light,is_local);
        HELPER(light,is_spot);
        HELPER(light,ambient);
        HELPER(light,color);
        HELPER(light,position);
        HELPER(light,half_vector);
        HELPER(light,cone_direction);
        HELPER(light,spot_cos_cutoff);
        HELPER(light,spot_exponent);
        HELPER(light,constant_attenuation);
        HELPER(light,linear_attenuation);
        HELPER(light,quadratic_attenuation);
    }
    for(i=0 ; i<locs->material_count ; ++i) {
        HELPER(material,is_affected_by_lights);
        HELPER(material,cubemap);
        HELPER(material,emission);
        HELPER(material,ambient);
        HELPER(material,diffuse);
        HELPER(material,specular);
        HELPER(material,shininess);
        HELPER(material,strength);
    }
#undef HELPER
}

void fe_gl_src_cube_locs_deinit(fe_gl_src_cube_locs *locs) {
    fe_mem_heapfree(locs->lights);
    fe_mem_heapfree(locs->materials);
}


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
            "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
                "precision highp float;\n"
            "#else\n"
                "precision mediump float;\n"
            "#endif\n"
        "#endif\n"
        "\n"
        "struct lightprops {\n"
        "    bool is_enabled; // do we apply this light in this invocation ?\n"
        "    bool is_local; // true for a point light or spotlight, false for a positional light;\n"
        "    bool is_spot; // true for a spotlight\n"
        "    vec3 ambient; // light's contribution to ambient light\n"
        "    vec3 color; // color of light\n"
        "    vec3 position; // if is_local, location of light. Otherwise, direction toward the light.\n"
        "    vec3 half_vector; // direction of hightlights for directional light\n"
        "    vec3 cone_direction;\n"
        "    float spot_cos_cutoff;\n"
        "    float spot_exponent;\n"
        "    float constant_attenuation; // local light attenuation coefficients\n"
        "    float linear_attenuation;\n"
        "    float quadratic_attenuation;\n"
        "    //etc.\n"
        "};\n"
        "struct materialprops {\n"
        "    bool is_affected_by_lights;\n"
        "    samplerCube cubemap;\n"
        "    vec3 emission;  // light produced by the material\n"
        "    vec3 ambient;   // what part of ambient light is reflected\n"
        "    vec3 diffuse;   // what part of diffuse light is scattered\n"
        "    vec3 specular;  // what part of specular light is scattered\n"
        "    float shininess; // exponent for sharpening specular reflection\n"
        "    float strength;\n"
        "    //etc.\n"
        "};\n"
        "\n"
        "#define LIGHT_MAX %u\n"
        "#define MATERIAL_MAX %u\n"
        "uniform lightprops u_lights[LIGHT_MAX];\n"
        "uniform materialprops u_materials[MATERIAL_MAX];\n"
        "uniform int u_material_for_front;\n"
        "uniform int u_material_for_back;\n"
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
        "void main() {\n"
        "    int m = (gl_FrontFacing ? u_material_for_front : u_material_for_back);\n"
        "    vec3 scattered_light = u_ambient_global.xyz;\n"
        "    vec3 reflected_light = vec3(0.0);\n"
        "    for(int l=0 ; l<LIGHT_MAX ; ++l) {\n"
        "        if(!u_lights[l].is_enabled\n"
        "        || !u_materials[m].is_affected_by_lights)\n"
        "            continue;\n"
        "        vec3 half_vector;\n"
        "        vec3 light_direction = u_lights[l].position;\n"
        "        float attenuation = 1.0;\n"
        "        if(u_lights[l].is_local) {\n"
        "            light_direction -= vec3(v_pos);\n"
        "            float light_distance = length(light_direction);\n"
        "            light_direction /= light_distance; \n"
        "            attenuation = 1.0 / (\n"
        "                u_lights[l].constant_attenuation\n"
        "              + u_lights[l].linear_attenuation    * light_distance\n"
        "              + u_lights[l].quadratic_attenuation * light_distance\n"
        "                                                  * light_distance\n"
        "            );\n"
        "            if(u_lights[l].is_spot) {\n"
        "                float spot_cos = dot(light_direction, -u_lights[l].cone_direction);\n"
        "                if(spot_cos < u_lights[l].spot_cos_cutoff)\n"
        "                    attenuation = 0.0;\n"
        "                else\n"
        "                    attenuation *= pow(spot_cos, u_lights[l].spot_exponent);\n"
        "            }\n"
        "            half_vector = normalize(light_direction + u_eye_direction);\n"
        "        } else {\n"
        "            half_vector = u_lights[l].half_vector;\n"
        "        }\n"
        "        float diffuse = max(0.0, dot(v_norm, light_direction));\n"
        "        float specular = max(0.0, dot(v_norm, half_vector));\n"
        "        if(diffuse == 0.0)\n"
        "            specular = 0.0;\n"
        "        else\n"
        "            specular = pow(specular, u_materials[m].shininess)\n"
        "                     * u_materials[m].strength;\n"
        "\n"
        "        scattered_light += u_lights[l].ambient*attenuation\n"
        "                        +  u_lights[l].color*attenuation*diffuse;\n"
        "        reflected_light += u_lights[l].color*attenuation*specular;\n"
        "    } //end for each enabled light\n"
        "\n"
        "    vec4 texel = textureCube(u_materials[m].cubemap, v_norm);\n"
        "    vec3 rgb = min(texel.rgb*scattered_light + reflected_light, vec3(1.0));\n"
        "    f_col = vec4(rgb, texel.a);\n"
        "}\n"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off",
        cfg->light_count, cfg->material_count,
        varying, varying
    );
}
