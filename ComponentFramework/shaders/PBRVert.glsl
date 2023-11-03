#version 450
#extension GL_ARB_separate_shader_objects : enable
#define MAX_SCENE_LIGHTS 5

layout(std140, binding = 0) uniform CameraMatrices{ // this is not a struct
    mat4 projection; // remember we filled in the projection matrix first into the UBO
    mat4 view;
};

layout(std140, binding = 1) uniform LightData{ 
    vec3 lightPos; // remember we filled in the position first
    vec4 colour;
};

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 eyeDir;
layout(location = 2) out vec2 texCoord;

layout(location = 3) out vec3 vertPos;
layout(location = 4) out vec3 vertDir;
layout(location = 5) out vec3 lightDir;
layout(location = 6) out vec4 camPos;
layout(location = 7) out vec3 worldVertPos;

layout(location = 20) uniform mat4 modelMatrix;

void main() {
    texCoord = uvCoord;
    texCoord.y *= -1;
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
	vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
    vertPos = vec3(view* modelMatrix * vVertex);
    vertDir = normalize(vertPos);
    eyeDir = -vertDir;

    lightDir = normalize(lightPos - vertPos);

    camPos = inverse(view) * vec4(vertPos, 1.0);

    worldVertPos = vec3( modelMatrix * vec4(vVertex) );

	gl_Position = projection * view * vec4(worldVertPos, 1.0);
}