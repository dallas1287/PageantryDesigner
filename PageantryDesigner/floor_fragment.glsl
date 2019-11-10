#version 440
in vec4 color;
in vec2 texCoord;
uniform sampler2D tex;
void main() 
{
   gl_FragColor = texture2D(tex, texCoord);
};