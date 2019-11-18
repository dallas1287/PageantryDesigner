#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoords;
//
//out vec2 TexCoords;
//
//void main()
//{
//    TexCoords = aTexCoords;
//    gl_Position = vec4(aPos, 1.0);
//}

in vec4 posAttr;
in vec3 texCoordAttr;

out vec2 TexCoords;

void main()
{
    TexCoords = texCoordAttr.xy;
    gl_Position = vec4(posAttr.xyz, 1.0);
}