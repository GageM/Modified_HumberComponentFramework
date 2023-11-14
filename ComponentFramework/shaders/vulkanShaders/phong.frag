#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 5

layout(binding = 2) uniform sampler2D texSampler;

layout (location = 0) in vec3 vertNormal;
layout (location = 1) in vec3 lightDir[MAX_LIGHTS];
layout (location = 2 + MAX_LIGHTS) in vec3 eyeDir; 
layout (location = 3 + MAX_LIGHTS) in vec2 fragTextCords;

layout(binding = 1) uniform GlobalLightingUBO {
	vec4 position[MAX_LIGHTS];
	vec4 diffuse[MAX_LIGHTS];
	vec4 specular[MAX_LIGHTS];
	vec4 ambient[MAX_LIGHTS];
	int lightCount;
} glights;

layout(push_constant) uniform PushConstants {
	mat4 modelMatrix;
} pushConstants;

layout (location = 0) out vec4 fragColor;

void main() { 
	float shinyness = 14.0;
	float ambMultiplier = 0.02;

	vec4 kt =  texture(texSampler, fragTextCords);

	// The rgba sums for each light
	fragColor = vec4(0.0, 0.0, 0.0, 0.0);

	for(int i = 0; i < glights.lightCount; i++) {
		float diff = max(dot(vertNormal, lightDir[i]), 0.0);
		/// Reflection is based incedent which means a vector from the light source
		/// not the direction to the light source
		vec3 reflection = normalize(reflect(-lightDir[i], vertNormal));
		float spec = max(dot(eyeDir, reflection), 0.0);
		if(diff > 0.0){
			spec = pow(spec, shinyness);
		}

		fragColor += (diff * (glights.diffuse[i] * kt));
		fragColor += (spec * glights.specular[i]);	
		fragColor += (ambMultiplier * glights.ambient[i] * kt);
	}
} 

