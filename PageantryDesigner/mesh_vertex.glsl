#version 440
in vec4 posAttr;
in vec4 colAttr;
in vec3 texCoordAttr;
in vec3 normAttr;
in vec4 boneTransform0;
in vec4 boneTransform1;
in vec4 boneTransform2;
in vec4 boneTransform3;
out vec4 color;

//change for light shader
//out vec3 norm;
out vec3 Normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//this is changed for lighting
out vec3 FragPos;
//this is added for lighttExture
out vec2 TexCoords;

void main() 
{
   mat4 boneTransform;
   boneTransform[0] = boneTransform0;
   boneTransform[1] = boneTransform1;
   boneTransform[2] = boneTransform2;
   boneTransform[3] = boneTransform3;

   FragPos = vec3(model * posAttr);

   vec4 bonePos = boneTransform * posAttr;
   gl_Position = projection * view * model * bonePos;
   TexCoords = texCoordAttr.xy;
   Normal = normAttr;
   color = colAttr;
};