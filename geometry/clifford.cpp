#include "clifford.h"
#include <glm/gtc/quaternion.hpp>
std::vector<Vertex> generateCliffordTorusVertices(uint32_t uSegments, uint32_t vSegments){
    std::vector<Vertex> vertices;
    vertices.reserve(uSegments * vSegments);

    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = glm::two_pi<float>() * float(i) / float(uSegments);
        for (uint32_t j = 0; j < vSegments; ++j) {
            float v = glm::two_pi<float>() * float(j) / float(vSegments);
            glm::vec4 pos(
                0.70710678f * cosf(u), //70710678f约等于1/sqrt(2)
                0.70710678f * sinf(u), 0.70710678f * cosf(v), 0.70710678f * sinf(v)
            );

            glm::vec3 color(cosf(u) * 0.5f + 0.5f, sinf(v) * 0.5f + 0.5f, 1.0f);

            vertices.emplace_back(pos, color);
        }
    }

    return vertices;
}
std::vector<Vertex> generateCliffordTorusVertices(uint32_t uSegments, uint32_t vSegments, float t) {
    std::vector<Vertex> vertices;
    vertices.reserve(uSegments * vSegments);

    int lobeCount = 3;
    float lobeDepth = t * 0.4f;

    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = glm::two_pi<float>() * float(i) / float(uSegments);
        
        // === S² 上的曲线插值 ===
        // t=0: 赤道大圆 (Clifford 对应的 S² 曲线)
        // t=1: 变形曲线 (Hopf torus)
        
        // 基曲线（大圆方向）
        glm::vec3 p_base(0.0f, cosf(u), sinf(u));
        
        // 变形曲线(带 lobe)
        float envelope = cosf(lobeDepth * cosf(lobeCount * u));
        glm::vec3 p_hopf(sinf(lobeDepth * cosf(lobeCount * u)), 
                         cosf(u) * envelope, 
                         sinf(u) * envelope);
        
        // 在 S² 上 slerp（用 glm::normalize + dot 检查）
        float dp = glm::dot(p_base, p_hopf);
        if (dp < 0.0f) p_hopf = -p_hopf;
        
        float angle = acosf(glm::clamp(glm::dot(p_base, p_hopf), -1.0f, 1.0f));
        glm::vec3 p_s2;
        if (angle < 0.001f) {
            p_s2 = p_base;
        } else {
            float sinAngle = sinf(angle);
            p_s2 = (sinf((1.0f - t) * angle) / sinAngle) * p_base 
                 + (sinf(t * angle) / sinAngle) * p_hopf;
        }

        // === Hopf 提升 ===
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
            glm::vec4 pos(
                cosv * y1,                    // w
                sinv * y1,                    // x  
                cosv * y2 - sinv * y3,        // y
                cosv * y3 + sinv * y2         // z
            );
            glm::vec3 color(
                cosf(u) * 0.5f + 0.5f,
                sinf(v) * 0.5f + 0.5f,
                glm::mix(1.0f, 0.8f, t)
            );

            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<uint16_t> generateCliffordTorusWireframeIndices(uint32_t uSegments, uint32_t vSegments){
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
std::vector<uint16_t> generateCliffordTorusIndices(uint32_t uSegments, uint32_t vSegments){
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
Clifford::Clifford(/* args */){
}

Clifford::~Clifford(){
}
void Clifford::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Clifford::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Clifford::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Clifford::Update(const void *useData){
    const UseData *parameter = (const UseData*)useData;
    const glm::uvec2 segments = glm::uvec2(64);
    std::vector<Vertex> vertices = generateCliffordTorusVertices(segments.x, segments.y, parameter->cliffordTime);
    std::vector<uint16_t> indices = generateCliffordTorusIndices(segments.x, segments.y);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    indices = generateCliffordTorusWireframeIndices(segments.x, segments.y);
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}