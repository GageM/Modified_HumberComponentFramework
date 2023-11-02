#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec4 vVertex;

layout(std140, binding = 0) uniform CameraMatrices{ // this is not a struct
    mat4 projection; // remember we filled in the projection matrix first into the UBO
    mat4 view;
};

layout(location = 0) out vec3 texCoord;

void main() {
    texCoord = vec3(-vVertex.x, vVertex.y, vVertex.z);
    
    vec4 pos = projection * view * vec4(vec3(vVertex), 1.0);

    gl_Position = pos.xyww;
}