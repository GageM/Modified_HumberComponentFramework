#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUVCoord;

layout(std140, binding = 0) uniform CameraMatrices{ // this is not a struct
    mat4 projection; // remember we filled in the projection matrix first into the UBO
    mat4 view;
};
layout(location = 1) uniform mat4 modelMatrix;


void main() {
    gl_Position = projection * view * modelMatrix * vec4(inVertex, 1.0);
}