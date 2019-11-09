#version 440

in vec3 norm;
in vec2 texCoord;
in vec4 color;
uniform sampler2D tex;

void main()
{
	gl_FragColor = texture2D(tex, texCoord);
}