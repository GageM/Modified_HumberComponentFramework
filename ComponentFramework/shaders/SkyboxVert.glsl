#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec4 vVertex;

layout(location = 0) out vec3 texCoord;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;


void main() {
    texCoord = vec3(-vVertex.x, vVertex.y, vVertex.z);
    
    vec4 pos = projectionMatrix * viewMatrix * vec4(vec3(vVertex), 1.0);

    gl_Position = pos.xyww;
}