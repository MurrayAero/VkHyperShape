#version 450
layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;
layout(location = 1) out float v_clip_v;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 projection;
    // float projection[5][5];//有一部分图形被裁掉了
} pc;

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
} ubo;
layout(set = 1, binding = 0, std430) readonly buffer mvpUniformBuffer {
    float view[5][5];
    float model[5][5];
    float projection[5][5];
} mvpubo;
void mat_vec_mul(float[5][5] m, float[5] v, out float[5] result) {
    result[0] = m[0][0]*v[0] + m[1][0]*v[1] + m[2][0]*v[2] + m[3][0]*v[3] + m[4][0]*v[4];
    result[1] = m[0][1]*v[0] + m[1][1]*v[1] + m[2][1]*v[2] + m[3][1]*v[3] + m[4][1]*v[4];
    result[2] = m[0][2]*v[0] + m[1][2]*v[1] + m[2][2]*v[2] + m[3][2]*v[3] + m[4][2]*v[4];
    result[3] = m[0][3]*v[0] + m[1][3]*v[1] + m[2][3]*v[2] + m[3][3]*v[3] + m[4][3]*v[4];
    result[4] = m[0][4]*v[0] + m[1][4]*v[1] + m[2][4]*v[2] + m[3][4]*v[3] + m[4][4]*v[4];
}
void mat_mul(float[5][5] a, float[5][5] b, out float[5][5] result) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            float sum = 0.0;
            for (int k = 0; k < 5; k++) {
                sum += a[k][i] * b[j][k];
            }
            result[j][i] = sum;
        }
    }
}
void mvp_transform(float[5][5]projection, float[5][5]view, float[5][5]model, out float[5][5]result){
    float mv[5][5];
    mat_mul(view, model, mv);
    mat_mul(projection, mv, result);
}
vec4 mvp_vec_transform(vec4 pos){
    float mvp[5][5];
    float point[5], inpos[5] = float[5](pos.x, pos.y, pos.z, pos.w, 1.0f);
    mvp_transform(mvpubo.projection, mvpubo.view, mvpubo.model, mvp);
    mat_vec_mul(mvp, inpos, point);
    return vec4(point[0], point[1], point[2], point[3]) / point[4];
}
void main() {
    outColor = inColor;
    vec4 point = mvp_vec_transform(inPos);
    v_clip_v = point.w;
	gl_Position = pc.projection * ubo.view * vec4(vec3(point), 1.0f);
}
