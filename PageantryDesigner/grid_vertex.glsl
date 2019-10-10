#version 440
in vec4 posAttr;
in vec4 colAttr;
out vec4 color;
uniform mat4 matrix;
void main() 
{
   color = colAttr;
   gl_Position = matrix * posAttr;
};