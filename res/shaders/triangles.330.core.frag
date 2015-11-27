#version 330 core

in vec4 vColor;
/* See http://stackoverflow.com/a/9222588 */
layout(location = 0) out vec4 fColor;

void main() 
{
    fColor = vec4(vColor.rgb, 1.0);
}
