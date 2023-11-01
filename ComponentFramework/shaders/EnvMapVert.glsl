#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

layout(location = 0) out vec2 texCoord;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;


void main() {
    texCoord = uvCoord;
    texCoord.y *= -1;
    
    vec4 pos = projectionMatrix * viewMatrix * vec4(vec3(vVertex), 1.0);

    gl_Position = pos;
}