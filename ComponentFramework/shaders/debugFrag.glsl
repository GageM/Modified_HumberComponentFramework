#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 2) uniform vec4 debugColor;
layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = debugColor;
}