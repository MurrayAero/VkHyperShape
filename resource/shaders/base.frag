#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in float v_clip_w;

layout(location = 0) out vec4 outColor;

void main() {
    // 这会导致复变函数图像缺失部分片段
    // if(v_clip_w <= 0.0) {
    //     discard;
    // }
    outColor = vec4(inColor, 1.0f);
}