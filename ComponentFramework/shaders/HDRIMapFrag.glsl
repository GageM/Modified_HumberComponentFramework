#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 localPos;

uniform sampler2D rawHDRI;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v);

void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(rawHDRI, uv).rgb;

    fragColor = vec4(color, 1.0);	
}

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}