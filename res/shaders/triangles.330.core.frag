#version 330 core

in vec4 vColor;
/* See http://stackoverflow.com/a/9222588 */
layout(location = 0) out vec4 fColor;
uniform bool ufInvert;

void main() 
{
    fColor = vec4(ufInvert ? 1.0-vColor.rgb : vColor.rgb, 0.6);
}
