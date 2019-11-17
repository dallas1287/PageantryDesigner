#version 440
//these should match the variables in GraphicsObject.h
const uint HAS_DIRECTIONAL_LIGHTS = 0x01;
const uint HAS_POINT_LIGHTS = 0x02;
const uint HAS_SPOTLIGHTS = 0x04;
const uint USES_LIGHTS = 0x08;
const uint USES_MATERIAL_TEXTURES = 0x10;
const uint HAS_COLOR_DATA = 0x20;

const vec4 DEFAULT_BLACK = vec4(0.0, 0.0, 0.0, 1.0);

struct Material {
    sampler2D diffuseTex;
    sampler2D specularTex;
	vec3 diffuse;
	vec3 specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
//in vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform int sceneData;
uniform bool blinn;

void main()
{           
    vec3 color = texture(material.diffuseTex, TexCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(pointLights[0].position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}