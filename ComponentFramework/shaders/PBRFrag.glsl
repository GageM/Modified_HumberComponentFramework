#version 450
#extension GL_ARB_separate_shader_objects : enable
#define MAX_SCENE_LIGHTS 5
#define PI 3.14159265359f

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 eyeDir; 
layout(location = 2) in vec2 texCoord;

layout(location = 3) in vec3 vertPos;
layout(location = 4) in vec3 vertDir;
layout(location = 5) in vec3 lightDir;
layout(location = 6) in vec4 camPos;
layout(location = 7) in vec3 worldVertPos;

layout(std140, binding = 1) uniform LightData{ 
    vec3 lightPos; // remember we filled in the position first
    vec4 colour;
};

layout(location = 0) uniform bool useBaseColorMap;
layout(location = 1) uniform bool useRoughnessMap;
layout(location = 2) uniform bool useMetallicMap;
layout(location = 3) uniform bool useNormalMap;

layout(location = 4) uniform vec4 baseColor_;
layout(location = 5) uniform float roughness_;
layout(location = 6) uniform float metallic_;

layout(location = 7) uniform sampler2D baseColorMap;
layout(location = 8) uniform sampler2D metallicMap;
layout(location = 9) uniform sampler2D roughnessMap;
layout(location = 10) uniform sampler2D normalMap;
//uniform sampler2D normalMap;

uniform samplerCube skybox;

// Function Declarations
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main() {
    // Set up material inputs    
    vec3 albedo;
    float metallic;
    float roughness;

    if(useBaseColorMap)  
    {
        albedo = texture(baseColorMap, texCoord).xyz;
    }
    else
    {
        albedo = baseColor_.rgb;
    }

    if(useRoughnessMap)  
    {
        roughness = clamp(float(texture(roughnessMap, texCoord)), 0.05, 0.99);
    }
    else 
    {
        roughness = clamp(roughness_, 0.05, 0.99);
    }

    if(useMetallicMap) 
    {
        metallic = float(texture(metallicMap, texCoord));
    }
    else 
    {
        metallic = metallic_;
    }


    // for cubeMap reflections
    //vec3 I = normalize(vertDir);
	//vec3 vertNormalFix = vec3(-vertNormal.x, vertNormal.y, vertNormal.z);
    //vec3 Reflection = reflect(I, vertNormalFix);
    //vec3 irradiance = vec3(texture(skybox, Reflection));

    // Normals
    vec3 N = normalize(vertNormal);
    vec3 V = normalize(vec3(-camPos.xyz) - worldVertPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // reflectance
    vec3 Lo = vec3(0.0);

    //per light radiance
    vec3 L = lightPos - worldVertPos;
    vec3 H = normalize(V + L); 
    float distance = length(lightPos - worldVertPos);
    float attenuation = 1.0/ (distance * distance);
    vec3 radiance = vec3(colour);// * attenuation; // Add Attenuation for physically accurate lights

    // cook-torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);// roughness);

    vec3 ks = F;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    // Clamp denominator to prevent divide by 0 errors
    float denominator = max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.0001);
    vec3 specular = numerator/denominator;

    float NdotL = max(dot(N, L), 0.0);
    Lo += (kd * albedo / PI + specular) * radiance * NdotL;
    
    //vec3 diffuse = albedo;
    vec3 ambient = vec3(0.03) * albedo;
    vec3 color = ambient + Lo;

    // gamma correction
    color = color / ( color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    fragColor = vec4(color, 1.0);
    

}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + ( (1 - F0) * pow(1.0 - cosTheta, 5) );
}

    