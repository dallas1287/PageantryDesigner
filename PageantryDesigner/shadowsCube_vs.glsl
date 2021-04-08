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
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool reverse_normals;

void main()
{
    vs_out.FragPos = vec3(model * vec4(posAttr, 1.0));
    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * normAttr);
    else
        vs_out.Normal = transpose(inverse(mat3(model))) * normAttr;
    vs_out.TexCoords = texCoordAttr.xy;
    gl_Position = projection * view * model * vec4(posAttr, 1.0);
}