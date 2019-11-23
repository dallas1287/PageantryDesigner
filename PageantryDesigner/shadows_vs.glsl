#version 440
in vec3 posAttr;
in vec4 colAttr;
in vec3 texCoordAttr;
in vec3 normAttr;

out vec2 TexCoords;

out VERTEX_DATA {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = vec3(model * vec4(posAttr, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * normAttr;
	//vs_out.Normal = normAttr;
    vs_out.TexCoords = texCoordAttr.xy;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(posAttr, 1.0);
}