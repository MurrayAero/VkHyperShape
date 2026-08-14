#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inUV;
layout(location = 1)in vec3 inNormal;
// //因为细分着色器用到所以不用也必须加...当然，目前只是验证层有反应
// layout (location = 2)in float inDiscard;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D inTex;

void main() {
	outColor = vec4(inUV, 1);
}
