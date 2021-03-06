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
in vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform int sceneData;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, bool usesTextures);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usesTextures);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usesTextures);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
   
	if((sceneData & USES_LIGHTS) != 0)
	{
		vec3 result = vec3(0.0, 0.0, 0.0);
		if((sceneData & HAS_DIRECTIONAL_LIGHTS) != 0)
			result = CalcDirLight(dirLight, norm, viewDir, (sceneData & USES_MATERIAL_TEXTURES) != 0);

		if((sceneData & HAS_POINT_LIGHTS) != 0)
		{
			for(int i = 0; i < NR_POINT_LIGHTS; i++)
				result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, (sceneData & USES_MATERIAL_TEXTURES) != 0);    
		}

		if((sceneData & HAS_SPOTLIGHTS) != 0)
			result += CalcSpotLight(spotLight, norm, FragPos, viewDir, (sceneData & USES_MATERIAL_TEXTURES) != 0);  
			
		gl_FragColor = vec4(result, 1.0);
    }
	else if((sceneData & HAS_COLOR_DATA) != 0)
	{
		gl_FragColor = color;
	}
	else
	{
		gl_FragColor = DEFAULT_BLACK;
	}
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, bool usesTextures)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * (usesTextures ? vec3(texture(material.diffuseTex, TexCoords)) : material.diffuse);
    vec3 diffuse = light.diffuse * diff * (usesTextures ? vec3(texture(material.diffuseTex, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (usesTextures ? vec3(texture(material.specularTex, TexCoords)) : material.specular);
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usesTextures)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * (usesTextures ? vec3(texture(material.diffuseTex, TexCoords)) : material.diffuse);
    vec3 diffuse = light.diffuse * diff * (usesTextures ? vec3(texture(material.diffuseTex, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (usesTextures ? vec3(texture(material.specularTex, TexCoords)) : material.specular);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usesTextures)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * (usesTextures ? vec3(texture(material.diffuseTex, TexCoords)) : material.diffuse);
    vec3 diffuse = light.diffuse * diff * (usesTextures ? vec3(texture(material.diffuseTex, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (usesTextures ? vec3(texture(material.specularTex, TexCoords)) : material.specular);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}