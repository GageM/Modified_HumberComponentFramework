#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 5

layout (location = 0) in  vec4 vVertex;
layout (location = 1) in  vec4 vNormal;
layout (location = 2) in  vec2 texCoords;

layout(binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform GlobalLightingUBO {
	vec4 position[MAX_LIGHTS];
	vec4 diffuse[MAX_LIGHTS];
	vec4 specular[MAX_LIGHTS];
	vec4 ambient[MAX_LIGHTS];
	int lightCount;
} glights;

layout(push_constant) uniform PushConstants {
	mat4 modelMatrix;
	mat4 normalMatrix;
} pushConstants;

layout (location = 0) out vec3 vertNormal;
layout (location = 1) out vec3 lightDir[MAX_LIGHTS];
layout (location = 2 + MAX_LIGHTS) out vec3 eyeDir; 
layout (location = 3 + MAX_LIGHTS) out vec2 fragTextCords;


void main() {
	fragTextCords = texCoords;
	//mat3 normalMatrix = mat3(inverse(transpose(pushConstants.modelMatrix)));
	vertNormal = normalize(mat3(pushConstants.normalMatrix) * vNormal.xyz); /// Rotate the normal to the correct orientation 
	vec3 vertPos = vec3(ubo.view * pushConstants.modelMatrix * vVertex); /// This is the position of the vertex from the origin
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;

	for(int i = 0; i < glights.lightCount; i++) {
	lightDir[i] = normalize(glights.position[i].xyz - vertPos); /// Create the light direction.
	}
	
	gl_Position =  ubo.proj * ubo.view * pushConstants.modelMatrix * vVertex; 
}
