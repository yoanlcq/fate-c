#version 330 core

uniform mat4 MVPMatrix;
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
out vec4 vColor;

void main()
{
    gl_Position = MVPMatrix * position;
    vColor = color;
}

