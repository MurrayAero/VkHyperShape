#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in float v_clip_v;

layout(location = 0) out vec4 outColor;

void main() {
    if(v_clip_v <= 0.0) {
        discard;
    }
    outColor = vec4(inColor, 1.0f);
}