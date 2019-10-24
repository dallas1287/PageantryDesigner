#version 440

in vec3 norm;
in vec2 TexCoord;
uniform sampler2D tex;

void main()
{
	vec3 lightDir = normalize(vec3(0.0, 1.0, 0.0));
	vec3 normal = normalize(norm);
	float intensity = max(dot(lightDir, normal), 0.0);
	gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0) * intensity;
}