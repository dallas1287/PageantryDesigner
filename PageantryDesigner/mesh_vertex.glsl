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
   //boneTransform[0] = boneTransform0;
   //boneTransform[1] = boneTransform1;
   //boneTransform[2] = boneTransform2;
   //boneTransform[3] = boneTransform3;

   boneTransform[0] = vec4(boneTransform0[0], boneTransform1[0], boneTransform2[0], boneTransform3[0]);
   boneTransform[1] = vec4(boneTransform0[1], boneTransform1[1], boneTransform2[1], boneTransform3[1]);
   boneTransform[2] = vec4(boneTransform0[2], boneTransform1[2], boneTransform2[2], boneTransform3[2]);
   boneTransform[3] = vec4(boneTransform0[3], boneTransform1[3], boneTransform2[3], boneTransform3[3]);
   vec4 bonePos = /*boneTransform **/ posAttr;
   gl_Position = projection * view * model * bonePos;
   texCoord = texCoordAttr;
   norm = normAttr;
};