#version 440
in vec4 posAttr;
in vec4 colAttr;
in vec2 texCoordAttr;
out vec4 color;
out vec2 texCoord;
uniform mat4 matrix;

void main() 
{
   color = colAttr;
   gl_Position = matrix * posAttr;
   texCoord = texCoordAttr;
};