#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec4 vVertex;

layout(std140, binding = 0) uniform CameraMatrices{ // this is not a struct
    mat4 projection; // remember we filled in the projection matrix first into the UBO
    mat4 view;
};

layout(location = 1) uniform mat4 modelMatrix;

layout(location = 0) out vec3 localPos;

void main() {
    localPos = vVertex.xyz;

    vec4 pos = projection * view * modelMatrix * vVertex;

    gl_Position = pos.xyww;
}