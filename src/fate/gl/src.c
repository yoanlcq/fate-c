#include <fate/gl/src.h>
#include <fate/timestamp.h>

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
        "uniform mat4 u_mvp;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = u_mvp * a_pos;\n"
        "    v_norm = normalize(a_pos.xyz);\n"
        "}\n"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off", 
        attr, varying
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
        "%s vec3 v_norm;\n"
        "\n"
        "#ifdef GL_ES\n"
            "#define f_col gl_FragColor\n"
        "#else\n"
            "out vec4 f_col;\n"
        "#endif//GL_ES\n"
        "\n"
        "uniform samplerCube u_cubemap;\n"
        "\n"
        "void main() {\n"
        "    f_col = texture(u_cubemap, v_norm);\n"
        "}\n"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off",
        varying
    );
}
