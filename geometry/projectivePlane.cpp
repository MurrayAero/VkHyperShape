#include "projectivePlane.h"
std::vector<Vertex>generateRealProjectivePlane(int uSegments = 64, int vSegments = 64){
    std::vector<Vertex> vertices;
    /*
        x = sin²θ * cos(2φ)
        y = sin²θ * sin(2φ)
        z = sin(2θ) * cos(φ)
        w = sin(2θ) * sin(φ)
    */
    vertices.reserve((uSegments + 1) * vSegments);
    for (int i = 0; i <= uSegments; ++i) {
        const float u = static_cast<float>(i) / uSegments;
        // θ ∈ [0, π/2]：上半球面
        const float theta = M_PI_2 * u;
        
        const float sinTheta   = std::sin(theta);
        const float sin2Theta  = std::sin(2.0f * theta);
        const float sinThetaSq = sinTheta * sinTheta;
        
        for (int j = 0; j < vSegments; ++j) {
            const float v = static_cast<float>(j) / vSegments;
            // φ ∈ [0, 2π]
            const float phi = glm::two_pi<float>() * v;
            // RP² → R⁴ 嵌入, 自动满足对径点等同
            glm::vec4 pos(
                sinThetaSq * std::cos(2.0f * phi),
                sinThetaSq * std::sin(2.0f * phi),
                sin2Theta  * std::cos(phi),
                sin2Theta  * std::sin(phi)
            );
            glm::vec3 color = pos * 0.5f + 0.5f;
            
            vertices.emplace_back(pos, color);
        }
    }
    
    return vertices;
}
void generateRealProjectivePlaneIndices(std::vector<uint16_t>&indices, std::vector<uint16_t>&lineIndices, int uSegments = 64, int vSegments = 64){
    indices.clear();
    lineIndices.clear();
    
    //v方向周期性, u=0 收缩为点, u=1对径点自动重合
    for (int i = 0; i < uSegments; ++i) {
        for (int j = 0; j < vSegments; ++j) {
            uint16_t a = static_cast<uint16_t>(i * vSegments + j);
            uint16_t b = static_cast<uint16_t>(i * vSegments + (j + 1) % vSegments);
            uint16_t c = static_cast<uint16_t>((i + 1) * vSegments + j);
            uint16_t d = static_cast<uint16_t>((i + 1) * vSegments + (j + 1) % vSegments);
            
            //逆时针
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);
            
            indices.push_back(c);
            indices.push_back(b);
            indices.push_back(d);
        }
    }
    
    // 水平线（纬线方向）
    for (int i = 0; i <= uSegments; ++i) {
        for (int j = 0; j < vSegments; ++j) {
            uint16_t a = static_cast<uint16_t>(i * vSegments + j);
            uint16_t b = static_cast<uint16_t>(i * vSegments + (j + 1) % vSegments);
            lineIndices.push_back(a);
            lineIndices.push_back(b);
        }
    }
    
    // 垂直线（经线方向）
    for (int i = 0; i < uSegments; ++i) {
        for (int j = 0; j < vSegments; ++j) {
            uint16_t a = static_cast<uint16_t>(i * vSegments + j);
            uint16_t c = static_cast<uint16_t>((i + 1) * vSegments + j);
            lineIndices.push_back(a);
            lineIndices.push_back(c);
        }
    }
}
ProjectivePlane::ProjectivePlane(/* args */){
}

ProjectivePlane::~ProjectivePlane(){
}
void ProjectivePlane::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void ProjectivePlane::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}
void ProjectivePlane::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void ProjectivePlane::Update(const void *useData){
    std::vector<uint16_t> indices, lineIndices;
    std::vector<Vertex> vertices = generateRealProjectivePlane();
    generateRealProjectivePlaneIndices(indices, lineIndices);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, lineIndices.data(), sizeof(uint16_t) * lineIndices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateIndexData(*gpu.device, lineIndices.data(), gpu.graphics, *gpu.pool);
    }
}