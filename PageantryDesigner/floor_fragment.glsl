#version 440
struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

in vec4 color;
in vec2 texCoord;
uniform Material material;

void main() 
{
   gl_FragColor = texture2D(material.diffuse, texCoord);
};