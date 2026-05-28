#include "hopf.h"
#include <complex>
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
            
            glm::vec3 color(r, 1.0f - r, float(j) / vSegments); 
            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<Vertex> generateHopfTorusVertices_Complex(uint32_t uSegments, uint32_t vSegments) {
    std::vector<Vertex> vertices;
    vertices.reserve(uSegments * vSegments);

    // 1. 这就是那个关键的 "r"
    // 当 r = 1/sqrt(2) 时，我们在 S³ 的 "赤道" 上切出了一个最胖的环面
    float r = CLIFFORD_R; 
    float r_prime = std::sqrt(1.0f - r * r); // 因为要在 S³ 上，所以 |z1| = sqrt(1-|z0|^2)

    for (uint32_t i = 0; i < uSegments; ++i) {
        // u 是第一个复数 z0 的相位 (Phase)
        float u = glm::two_pi<float>() * float(i) / float(uSegments);
        
        // 2. 构造第一个复数 z0 = r * e^(i*u)
        // 它的模长固定为 r，相位是 u
        std::complex<float>z0 = std::polar(r, u);

        for (uint32_t j = 0; j < vSegments; ++j) {
            float v = glm::two_pi<float>() * float(j) / float(vSegments);

            // 3. 构造第二个复数 z1 = r' * e^(i*v)
            // 它的模长固定为 r'，相位是 v
            std::complex<float>z1 = std::polar(r_prime, v);

            // 4. 这就是 S³ 上的点！
            // 因为 |z0|^2 + |z1|^2 = r^2 + (1-r^2) = 1
            // 它完美符合 S³ 的定义
            glm::vec4 pos(z0.real(), z0.imag(), z1.real(), z1.imag());

            // 颜色逻辑：通常我们用 u 和 v 来着色，方便观察
            // u 代表 Hopf 纤维的方向，v 代表环面管状的方向
            glm::vec3 color(cosf(u) * 0.5f + 0.5f, sinf(v) * 0.5f + 0.5f, 1.0f);
            
            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<Vertex> generateHopfTorusVertices(uint32_t uSegments, uint32_t vSegments){
    std::vector<Vertex> vertices;
    vertices.reserve(uSegments * vSegments);
    float sqrt_inv = CLIFFORD_R; //保证点在 S^3 上
    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = glm::two_pi<float>() * float(i) / float(uSegments);
        for (uint32_t j = 0; j < vSegments; ++j) {
            float v = glm::two_pi<float>() * float(j) / float(vSegments);
            glm::vec4 pos(
                sqrt_inv * cosf(u),
                sqrt_inv * sinf(u), sqrt_inv * cosf(v), sqrt_inv * sinf(v)
            );
            glm::vec3 color(cosf(u) * 0.5f + 0.5f, sinf(v) * 0.5f + 0.5f, 1.0f);
#ifdef DEBUG
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
    // std::vector<Vertex>vertices;
    // for (int i = 0; i < segments.y; ++i) {
    //     // 让 r 从 0.1 (靠近南极) 到 10 (靠近北极)
    //     // 用指数或线性映射都可以，指数能更集中在极点附近
    //     float r = powf(10.0f, (float)i / (segments.y - 1) * 2.0f - 1.0f); 
    //     // 或者用线性: float r = 0.1f + 10.0f * (float)i / (segments.y - 1);
    //     for (int j = 0; j < segments.x; ++j) {
    //         float k_offset = glm::two_pi<float>() * (float)j / segments.x;
    //         auto fiber = generateFullHopfFiber(r, k_offset, segments.y);
    //         vertices.insert(vertices.end(), fiber.begin(), fiber.end());
    //     }
    // }
    std::vector<Vertex> vertices;
    if(parameter->hopf.clifford){
        vertices = generateHopfTorusLayer(CLIFFORD_R, segments.x, segments.y);
    }
    else{
        vertices = generateHopfTorusLayer(parameter->hopf.r, segments.x, segments.y);
    }
    // std::vector<Vertex> vertices = generateHopfTorusVertices(segments.x, segments.y, parameter->hopf.r);
    // std::vector<Vertex> vertices = generateHopfTorusVertices_Complex(segments.x, segments.y);
    std::vector<uint16_t> indices = generateHopfTorusIndices(segments.x, segments.y);
    // std::vector<Vertex> vertices = generateHopfTorusVertices(segments.x, segments.y, parameter->Hopf.lobeCount, parameter->hopf.r);
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