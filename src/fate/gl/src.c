#include <fate/gl/src.h>
#include <fate/timestamp.h>

static fe_timestamp timestamp = 0;

fe_timestamp fe_gl_src_get_build_timestamp(void) {
    return timestamp ? timestamp : (timestamp = fe_timestamp_here());
}

void fe_gl_src_before_linking(const fe_gl_shader_source_set *ss, GLuint prog, fe_timestamp last_build_time) {
	glBindAttribLocation(prog, 0, "a_pos");
	glBindAttribLocation(prog, 1, "a_col");
}

/*

const char *const fe_gl_src_tri_130_vert =
	"// tri.130.vert\n"
    "#version 130\n"
    "\n"
    "in vec4 a_pos;\n"
    "in vec4 a_col;\n"
    "out vec4 v_col;\n"
    "uniform mat4 u_mvp;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = u_mvp * a_pos;\n"
    "    v_col = a_col;\n"
    "}\n"
;

const char *const fe_gl_src_tri_130_frag =
	"// tri.130.frag\n"
    "#version 130\n"
    "\n"
    "in vec4 v_col;\n"
    "out vec4 f_col;\n"
	"uniform bool u_invert;\n"
    "\n"
    "void main() {\n"
    "    f_col = vec4(u_invert ? 1.0-v_col.rgb : v_col.rgb, 0.6);\n"
    "}"
;

*/



void fe_gl_src_get_tri_vert(fe_iov *iov, const fe_gl_src_config *cfg) {

    unsigned version = cfg->es ? 100 : 130;
    const char *attr = cfg->es ? "attribute" : "in";
    const char *varying = cfg->es ? "varying" : "out";
    fe_iov_printf(iov, iov->len, 
        "// tri.%u%s.vert\n"
        "#version %u\n"
        "\n"
        "#pragma optimize(%s)\n"
        "#pragma debug(%s)\n"
        "\n"
        "%s vec4 a_pos;\n"
        "%s vec4 a_col;\n"
        "%s vec4 v_col;\n"
        "uniform   mat4 u_mvp;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = u_mvp * a_pos;\n"
        "    v_col = a_col;\n"
        "}\n"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off", 
        attr, attr, varying
    );
}

void fe_gl_src_get_tri_frag(fe_iov *iov, const fe_gl_src_config *cfg) {

    unsigned version = cfg->es ? 100 : 130;
    fe_iov_printf(iov, iov->len, 
        "// tri.%u%s.frag\n"
        "#version %u\n"
        "\n"
        "#pragma optimize(%s)\n"
        "#pragma debug(%s)\n"
        "\n"
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "in vec4 v_col;\n"
        "#ifdef GL_ES\n"
        "#define f_col gl_FragColor\n"
        "#else\n"
        "out vec4 f_col;\n"
        "#endif//GL_ES\n"
        "uniform bool u_invert;\n"
        "\n"
        "void main() {\n"
        "    f_col = vec4(u_invert ? 1.0-v_col.rgb : v_col.rgb, 0.6);\n"
        "}"
        ,
        version, cfg->es ? "es" : "", version,
        cfg->optimize ? "on" : "off", 
        cfg->debug    ? "on" : "off"
    );
}
