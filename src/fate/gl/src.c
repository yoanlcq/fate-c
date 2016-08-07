#include <fate/gl/src.h>
#include <fate/timestamp.h>

static fe_timestamp timestamp = 0;

fe_timestamp fe_gl_src_get_build_timestamp(void) {
    return timestamp ? timestamp : (timestamp = fe_timestamp_here());
}

const char *const fe_gl_src_tri_330_vert =
    "#version 110\n"
    "#extension GL_ARB_explicit_attrib_location : require\n"
    //"#version 330 core\n"
    "\n"
    "uniform mat4 MVPMatrix;\n"
    "layout(location = 0) in vec4 position;\n"
    "layout(location = 1) in vec4 color;\n"
    "varying out vec4 vColor;\n"
    "\n"
    "void main() {\n"
    "    gl_Position = MVPMatrix * position;\n"
    "    vColor = color;\n"
    "}\n"
;

const char *const fe_gl_src_tri_330_frag =
    "#version 110\n"
    "#extension GL_ARB_explicit_attrib_location : require\n"
    //"#version 330 core\n"
    "\n"
    "in vec4 vColor;\n"
    "/* See http://stackoverflow.com/a/9222588 */\n"
    "layout(location = 0) varying out vec4 fColor;\n"
    "uniform bool ufInvert;\n"
    "\n"
    "void main() {\n"
    "    fColor = vec4(ufInvert ? 1.0-vColor.rgb : vColor.rgb, 0.6);\n"
    "}"
;
