#version 440
in vec4 posAttr;
in vec4 colAttr;
in vec2 texCoordAttr;
out vec4 color;
out vec2 texCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
   color = colAttr;
   gl_Position = projection * view * model * posAttr;
   texCoord = texCoordAttr;
};