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

    const float PI2 = glm::two_pi<float>();
    const float SQRT2_INV = 0.70710678f;

    for (uint32_t i = 0; i < uSegments; ++i) {
        float u = PI2 * float(i) / float(uSegments);
        
        for (uint32_t j = 0; j < vSegments; ++j) {
            // 统一使用 v ∈ [0, 4π]
            float v = 4.0f * PI2 * float(j) / float(vSegments);
            
            // 1. Clifford (t=0) - 调整公式以适应 v ∈ [0, 4π]
            // 为了让 Clifford 在 v ∈ [0, 4π] 时形状不变，我们需要将 v 减半
            float v_clifford = v * 0.5f; // v_clifford ∈ [0, 2π]
            glm::vec4 clifford(
                SQRT2_INV * cosf(u),
                SQRT2_INV * sinf(u),
                SQRT2_INV * cosf(v_clifford),  // 使用 v_clifford
                SQRT2_INV * sinf(v_clifford)   // 使用 v_clifford
            );

            // 2. Hopf (t=1) - 使用完整的 v ∈ [0, 4π]
            float halfV = v * 0.5f;  // halfV ∈ [0, 2π]
            glm::vec4 hopf(
                cosf(halfV) * cosf(u),
                cosf(halfV) * sinf(u),
                sinf(halfV) * cosf(u),
                sinf(halfV) * sinf(u)
            );

            // 3. Quaternion 转换 + Slerp
            glm::quat clifford_quat(clifford.w, clifford.x, clifford.y, clifford.z);
            glm::quat hopf_quat(hopf.w, hopf.x, hopf.y, hopf.z);

            clifford_quat = glm::normalize(clifford_quat);
            hopf_quat = glm::normalize(hopf_quat);

            glm::quat result_quat;
            if (t <= 0.0f) {
                result_quat = clifford_quat;
            } else if (t >= 1.0f) {
                result_quat = hopf_quat;
            } else {
                result_quat = glm::slerp(clifford_quat, hopf_quat, t);
            }

            glm::vec4 pos(result_quat.x, result_quat.y, result_quat.z, result_quat.w);

            glm::vec3 color(cosf(u) * 0.5f + 0.5f, sinf(v) * 0.5f + 0.5f, 1.0f);
            
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