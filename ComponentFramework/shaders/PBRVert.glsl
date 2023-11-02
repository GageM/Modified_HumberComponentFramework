#version 450
#extension GL_ARB_separate_shader_objects : enable
#define MAX_SCENE_LIGHTS 5

struct Light
{
    vec4 diffuseColor;
    vec3 position;
    vec3 direction;
};

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 eyeDir;
layout(location = 2) out vec2 texCoord;

layout(location = 3) out vec3 vertPos;
layout(location = 4) out vec3 vertDir;

layout(location = 5) out int fNumLights;
layout(location = 6) out Light fLights[MAX_SCENE_LIGHTS];

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;

uniform int vNumLights;
uniform Light vLights[MAX_SCENE_LIGHTS];

void main() {
    texCoord = uvCoord;
    texCoord.y *= -1;
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
    vertPos = vec3(viewMatrix * modelMatrix * vVertex);
    vertDir = normalize(vertPos);
    eyeDir = -vertDir;

    fNumLights = vNumLights;
    fLights = vLights;


    for(int i = 0; i < vNumLights; i++)
    {
        fLights[i].direction = normalize(vLights[i].position - vertPos);
    }

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vVertex;
}