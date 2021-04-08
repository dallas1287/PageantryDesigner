#version 440
in vec3 posAttr;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(posAttr, 1.0);
}