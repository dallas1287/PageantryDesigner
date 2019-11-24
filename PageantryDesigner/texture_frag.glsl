#version 440

in vec2 TexCoords;
uniform sampler2D diffuseTexture;

void main()
{
	gl_FragColor = texture2D(diffuseTexture, TexCoords);
}