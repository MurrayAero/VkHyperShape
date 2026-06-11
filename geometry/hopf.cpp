#include <complex>

#include "hopf.h"
std::vector<Vertex> generateHopfTorusLayer(float r, uint32_t uSegments, uint32_t vSegments) {
    std::vector<Vertex> vertices;
    float r_prime = std::sqrt(1.0f - r * r); 

    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = glm::two_pi<float>() * float(i) / float(uSegments);
        std::complex<float>z0 = std::polar(r, u); // z0 的模长现在是 r
        for (uint32_t j = 0; j < vSegments; ++j) {
            float v = glm::two_pi<float>() * float(j) / float(vSegments);
            std::complex<float>z1 = std::polar(r_prime, v); // z1 的模长现在是 r_prime

            glm::vec4 pos(z0.real(), z0.imag(), z1.real(), z1.imag());
            glm::vec3 color(r, 1.0f - r, j / vSegments); 
#ifndef NDEBUG
            if(pos.w >= -0.1f) color = glm::vec3(0,0,1);
            else color = glm::vec3(1,0,0);
#endif
            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<Vertex> generateHopfTorusVertices(uint32_t uSegments, uint32_t vSegments, int32_t lobeCount, float t) {
    std::vector<Vertex> vertices;
    vertices.reserve(uSegments * vSegments);

    float lobeDepth = t * 0.4f;

    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = glm::two_pi<float>() * float(i) / float(uSegments);
        
        // === S² 上的曲线插值 ===
        // t=0: 赤道大圆(Clifford 对应的S²曲线)
        // t=1: 变形曲线(Hopf torus)
        
        //大圆方向的点
        glm::vec3 p_base(0.0f, cosf(u), sinf(u));
        
        // 变形曲线(带 lobe)
        float envelope = cosf(lobeDepth * cosf(lobeCount * u));
        glm::vec3 p_hopf(sinf(lobeDepth * cosf(lobeCount * u)), cosf(u) * envelope, sinf(u) * envelope);
        float dp = glm::dot(p_base, p_hopf);
        if (dp < 0.0f) p_hopf = -p_hopf;
        
        float angle = acosf(glm::clamp(glm::dot(p_base, p_hopf), -1.0f, 1.0f));
        glm::vec3 p_s2;
        if (angle < 0.001f) {
            p_s2 = p_base;
        } else {
            float sinAngle = sinf(angle);
            p_s2 = (sinf((1.0f - t) * angle) / sinAngle) * p_base + (sinf(t * angle) / sinAngle) * p_hopf;
        }

        // p_s2 = (p1, p2, p3)，构造水平提升 y(u)
        float yden = sqrtf(2.0f * (1.0f + p_s2.x));
        float y1 = (1.0f + p_s2.x) / yden;
        float y2 = p_s2.y / yden;
        float y3 = p_s2.z / yden;

        for (uint32_t j = 0; j < vSegments; ++j) {
            float v = glm::two_pi<float>() * float(j) / float(vSegments);
            float cosv = cosf(v);
            float sinv = sinf(v);

            // e^(i*v) * y(u) 作为四元数乘法
            glm::vec4 pos(cosv * y1, sinv * y1, cosv * y2 - sinv * y3, cosv * y3 + sinv * y2);
            glm::vec3 color(cosf(u) * 0.5f + 0.5f, sinf(v) * 0.5f + 0.5f, glm::mix(1.0f, 0.8f, t));

            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<Vertex> generateHybridHopf(float r, float t, uint32_t uSegments, uint32_t vSegments, uint32_t lobeCount) {
    std::vector<Vertex> vertices;
    vertices.reserve(uSegments * vSegments);

    float r_prime = std::sqrt(1.0f - r * r); // r' = sqrt(1 - r^2)
    float lobeDepth = t * 0.4f; // t 控制变形程度

    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = glm::two_pi<float>() * float(i) / float(uSegments);

        // --- 第一部分：定义 S² 上的曲线 (来自第二份代码，由 t 控制) ---
        glm::vec3 p_base(0.0f, cosf(u), sinf(u));
        
        // 变形曲线
        float envelope = cosf(lobeDepth * cosf(lobeCount * u));
        glm::vec3 p_hopf(sinf(lobeDepth * cosf(lobeCount * u)), cosf(u) * envelope, sinf(u) * envelope);
        
        // 防止翻转
        if (glm::dot(p_base, p_hopf) < 0.0f) p_hopf = -p_hopf;

        // Slerp 插值
        float angle = acosf(glm::clamp(glm::dot(p_base, p_hopf), -1.0f, 1.0f));
        glm::vec3 p_s2;
        if (angle < 0.001f) {
            p_s2 = p_base;
        } else {
            float sinAngle = sinf(angle);
            p_s2 = (sinf((1.0f - t) * angle) / sinAngle) * p_base + (sinf(t * angle) / sinAngle) * p_hopf;
        }

        // --- 第二部分：结合 r 参数进行 Hopf 提升 ---
        // 这里的 p_s2 是 S² 上的点，我们需要把它映射回 S³，并且满足 |z0| = r
        
        // 方法：使用 p_s2 作为 Hopf 映射的方向，并用 r 和 r' 作为模长
        // Hopf 映射公式: pi(q) = (2*q0*q2, 2*q1*q2, q0^2 + q1^2 - q2^2 - q3^2) 的逆过程比较复杂
        // 简化方案：利用第一份代码的复数对思想，但用 p_s2 生成相位
        
        // 将 p_s2 转换为 Hopf 坐标 (z0, z1)
        // 这里的技巧是：让 z0 的相位是 u，模长是 r；z1 的相位是 v，模长是 r'，但方向受 p_s2 影响
        // 更直接的方法：使用四元数旋转
        
        // 简化实现：直接利用 r 和 p_s2 构造 Hopf 纤维
        // 假设 p_s2 给出了 Hopf 纤维在 S² 上的投影点，我们需要恢复对应的 S³ 点
        // 标准构造：对于 S² 上的点 (x,y,z)，对应的 Hopf 纤维之一是：
        // z0 = sqrt((1+x)/2) * e^(i*u)
        // z1 = sqrt((1-x)/2) * e^(i*(u+phase)) * (y + i*z) / sqrt(1-x^2) ... (略复杂)
        
        // 为了结合 r，我们采用另一种直观方式：
        // 将 p_s2 视为法向量，构造一个圆截面
        
        // 实际上，你的第二份代码已经生成了 Hopf 环面，我们只需要把 r 加进去缩放
        // 修改第二份代码中的 y(u) 构造部分，使其满足 |z0|=r, |z1|=r'
        
        // 这是一个简化的结合版本（保持 Hopf 结构，引入 r 缩放）：
        float yden = sqrtf(2.0f * (1.0f + p_s2.x));
        float y1 = (1.0f + p_s2.x) / yden; // 这对应 Re(z0) 的系数
        float y2 = p_s2.y / yden;         // 这对应 Im(z0) 或 Re(z1) 的系数
        float y3 = p_s2.z / yden;         // 这对应 Im(z1) 的系数

        // 引入 r 和 r' 进行缩放（关键步骤）
        // 我们希望 z0 的模是 r，z1 的模是 r'
        // 原代码默认是 Clifford 环面 (r=1/sqrt(2))，现在我们要缩放它
        // 缩放因子 alpha 作用于 z0，beta 作用于 z1
        // 为了不破坏 Hopf 结构，我们缩放 y 向量
        float scale_z0 = r; 
        float scale_z1 = r_prime;
        
        // 重新分配 y 向量的贡献
        // y1 主要影响 z0 的实部，y2/y3 主要影响 z1
        // 这是一个近似的结合，用于展示效果
        float new_y1 = y1 * scale_z0;
        float new_y2 = y2 * scale_z1;
        float new_y3 = y3 * scale_z1;

        for (uint32_t j = 0; j < vSegments; ++j) {
            float v = glm::two_pi<float>() * float(j) / float(vSegments);
            float cosv = cosf(v);
            float sinv = sinf(v);

            // 使用新的 y 向量构造 S³ 点
            // pos = (Re(z0), Im(z0), Re(z1), Im(z1))
            // 这里沿用之前的复数乘法结构，但应用了新的缩放
            glm::vec4 pos(
                cosv * new_y1 * scale_z0, // 稍微调整以确保模长正确
                sinv * new_y1 * scale_z0,
                cosv * new_y2 - sinv * new_y3,
                cosv * new_y3 + sinv * new_y2
            );
            
            // 归一化以确保严格在 S³ 上 (可选，因为浮点误差)
            // pos = glm::normalize(pos);

            glm::vec3 color(r, t, float(j) / vSegments); // 颜色反馈 r 和 t
            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<uint16_t> generateHopfTorusEdge(uint32_t uSegments, uint32_t vSegments){
    std::vector<uint16_t> lineIndices;
    lineIndices.reserve(uSegments * vSegments * 4);

    auto index = [&](uint16_t i, uint16_t j) {
        return (i % uSegments) * vSegments + (j % vSegments);
    };

    for (uint16_t i = 0; i < uSegments; ++i) {
        for (uint16_t j = 0; j < vSegments; ++j) {
            uint16_t i1 = index(i, j);
            uint16_t i2 = index(i + 1, j);
            uint16_t i3 = index(i, j + 1);

            lineIndices.push_back(i1);
            lineIndices.push_back(i2);

            lineIndices.push_back(i1);
            lineIndices.push_back(i3);
        }
    }
    
    return lineIndices;
}
std::vector<uint16_t> generateHopfTorusIndices(uint32_t uSegments, uint32_t vSegments){
    std::vector<uint16_t> indices;
    indices.reserve(uSegments * vSegments * 6);

    auto index = [&](uint16_t i, uint16_t j) {
        return (i % uSegments) * vSegments + (j % vSegments);
    };

    for (uint16_t i = 0; i < uSegments; ++i) {
        for (uint16_t j = 0; j < vSegments; ++j) {

            uint16_t i1 = index(i, j);
            uint16_t i2 = index(i + 1, j);
            uint16_t i3 = index(i, j + 1);
            uint16_t i4 = index(i + 1, j + 1);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);

            indices.push_back(i3);
            indices.push_back(i2);
            indices.push_back(i4);
        }
    }

    return indices;
}
Hopf::Hopf(/* args */){
}

Hopf::~Hopf(){
}
void Hopf::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Hopf::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Hopf::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Hopf::Update(const void *useData){
    const glm::uvec2 segments = glm::uvec2(64);
    const UseData *parameter = (const UseData*)useData;
    std::vector<Vertex> vertices;
    if(parameter->hopf.clifford){
        vertices = generateHopfTorusLayer(CLIFFORD_R, segments.x, segments.y);
        // vertices = generateHybridHopf(CLIFFORD_R, parameter->hopf.time, segments.x, segments.y, parameter->hopf.lobeCount);
    }
    else{
        vertices = generateHopfTorusLayer(parameter->hopf.torusAspect, segments.x, segments.y);
        // vertices = generateHybridHopf(parameter->hopf.torusAspect, parameter->hopf.time, segments.x, segments.y, parameter->hopf.lobeCount);
    }
    // std::vector<Vertex> vertices = generateHopfTorusVertices(segments.x, segments.y, parameter->hopf.torusAspect);
    // std::vector<Vertex> vertices = generateHopfTorusVertices_Complex(segments.x, segments.y);
    std::vector<uint16_t> indices = generateHopfTorusIndices(segments.x, segments.y);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    indices = generateHopfTorusEdge(segments.x, segments.y);
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}