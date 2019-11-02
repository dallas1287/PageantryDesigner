#version 440
in vec4 posAttr;
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
   mat4 boneTransform;
   boneTransform[0] = boneTransform0;
   boneTransform[1] = boneTransform1;
   boneTransform[2] = boneTransform2;
   boneTransform[3] = boneTransform3;

   vec4 bonePos = boneTransform * posAttr;
   gl_Position = projection * view * model * bonePos;
   texCoord = texCoordAttr;
   norm = normAttr;
};