#version 450
#extension GL_ARB_separate_shader_objects : enable
#define MAX_SCENE_LIGHTS 5
#define PI 3.14159265359f

struct Light
{
    vec4 diffuseColor;
    vec3 position;
    vec3 direction;
};

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 eyeDir; 
layout(location = 2) in vec2 texCoord;

layout(location = 3) in vec3 vertPos;
layout(location = 4) in vec3 vertDir;

layout(location = 5) flat in int fNumLights;
layout(location = 6) in Light fLights[MAX_SCENE_LIGHTS];

layout(location = 30) uniform bool useBaseColorMap;
layout(location = 4) uniform bool useRoughnessMap;
layout(location = 5) uniform bool useMetallicMap;
layout(location = 6) uniform bool useNormalMap;

layout(location = 32) uniform vec4 baseColor_;
layout(location = 8) uniform float roughness_;
layout(location = 9) uniform float metallic_;

layout(location = 31) uniform sampler2D baseColorMap;
layout(location = 11) uniform sampler2D metallicMap;
layout(location = 12) uniform sampler2D roughnessMap;
layout(location = 13) uniform sampler2D normalMap;
//uniform sampler2D normalMap;

uniform samplerCube skybox;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness);

void main() {

    vec3 albedo;
    float metallic;
    float roughness;

    //if(useBaseColorMap)
    //{
    //albedo = vec3(texture(baseColorMap, texCoord)) * baseColor_.rgb;
    //}
    //else 
    //{
    //albedo = baseColor_.rgb;
    //}

    albedo = baseColor_.rgb;

    if(useRoughnessMap)  // Clamping roughness to prevent shader artifacts at extreme values
    {
    roughness = clamp(float(texture(roughnessMap, texCoord)), 0.05, 0.99) * roughness_;
    }
    else 
    {
    roughness = clamp(roughness_, 0.05, 0.99);
    }

    if(useMetallicMap) 
    {
    metallic = float(texture(metallicMap, texCoord)) * metallic_;
    }
    else 
    {
    metallic = metallic_;
    }


    // for cubeMap reflections
    vec3 I = normalize(vertDir);
	vec3 vertNormalFix = vec3(-vertNormal.x, vertNormal.y, vertNormal.z);
    vec3 Reflection = reflect(I, vertNormalFix);
    vec3 irradiance = vec3(texture(skybox, Reflection));

    // Normals
    vec3 N = normalize(vertNormal);
    vec3 V = normalize(eyeDir);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // reflectance
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < fNumLights; i++)
    {
    //per light radiance
        vec3 L = fLights[i].direction;
        vec3 H = normalize(fLights[i].direction + eyeDir); 
        float distance = length(fLights[i].position - vertPos);
        float attenuation = 1.0/ (distance * distance);
        vec3 radiance = vec3(fLights[i].diffuseColor);// * attenuation; // Add Attenuation for physically accurate lights

        // cook-torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0, roughness);

        vec3 ks = F;
        vec3 kd = vec3(1.0) - ks;
        kd *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator/denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kd * albedo / PI + specular) * radiance * NdotL;
    }
    
    vec3 diffuse = albedo;
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

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

    