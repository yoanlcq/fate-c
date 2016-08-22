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

const char *const fe_gl_src_tri_130_vert =
    "#version 130\n"
    "\n"
    "in vec4 a_pos;\n"
    "in vec4 a_col;\n"
    "varying out vec4 v_col;\n"
    "uniform mat4 u_mvp;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = u_mvp * a_pos;\n"
    "    v_col = a_col;\n"
    "}\n"
;

const char *const fe_gl_src_tri_130_frag =
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
