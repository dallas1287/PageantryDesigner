#version 440
in vec3 posAttr;
in vec4 colAttr;
in vec2 texCoordAttr;
in vec3 normAttr;
in vec4 boneTransform0;
in vec4 boneTransform1;
in vec4 boneTransform2;
in vec4 boneTransform3;
out vec4 color;
out vec2 texCoord;
out vec3 norm;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
   color = colAttr;
   mat4 boneTransform = mat4(boneTransform0, boneTransform1, boneTransform2, boneTransform3);
   gl_Position = projection * view * model * boneTransform * vec4(posAttr, 1.0);
   texCoord = texCoordAttr;
   norm = normAttr;
};