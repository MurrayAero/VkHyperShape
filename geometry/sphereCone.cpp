#include "sphereCone.h"

std::vector<Vertex> generateSphereConeVertices(float height, float theta,// 圆锥半角
    int segments,// 经度细分
    int rings,// 纬度细分
    float radius = 1.0f
) {
    std::vector<Vertex> vertices;
    
    // 索引 0: 四维顶点
    vertices.push_back(Vertex(glm::vec4(0.0f), glm::vec3(1.0f, 0.2f, 0.2f)));
    
    // 索引 1: 底面三维球体的球心
    vertices.push_back(Vertex(glm::vec4(0.0f, 0.0f, 0.0f, height), glm::vec3(0.2f, 1.0f, 0.2f)));
    
    // 索引 2 开始: 底面球面上的顶点
    for (int i = 0; i <= rings; ++i) {
        float theta = M_PI * i / rings; // 纬度 [0, PI]
        for (int j = 0; j <= segments; ++j) {
            float phi = 2.0f * M_PI * j / segments; // 经度 [0, 2PI]
            
            float x = radius * std::sin(theta) * std::cos(phi);
            float y = radius * std::sin(theta) * std::sin(phi);
            float z = radius * std::cos(theta);
            float w = height;

            glm::vec4 pos = glm::vec4(x, y, z, w);
            
            vertices.push_back(Vertex(pos, pos * .5f + .5f));
        }
    }

    return vertices;
}
std::vector<uint16_t> generateSphereConeIndices(int segments, int rings) {
    std::vector<uint16_t> indices;
    
    const uint16_t apexIdx = 0;
    const uint16_t centerIdx = 1;
    
    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < segments; ++j) {
            // 计算球面网格上的四个点
            uint16_t p00 = 2 + i * (segments + 1) + j;
            uint16_t p10 = 2 + (i + 1) * (segments + 1) + j;
            uint16_t p01 = 2 + i * (segments + 1) + (j + 1);
            uint16_t p11 = 2 + (i + 1) * (segments + 1) + (j + 1);
            
            // 构成球面网格的一个四边形分为两个三角形：T1(p00, p10, p01) 和 T2(p10, p11, p01)
            
            // 侧面胞腔：顶点 + 球面三角形 -> 四面体
            indices.push_back(apexIdx); indices.push_back(p00); indices.push_back(p10); indices.push_back(p01);
            indices.push_back(apexIdx); indices.push_back(p10); indices.push_back(p11); indices.push_back(p01);
            
            // 底面胞腔：球心 + 球面三角形 -> 四面体
            indices.push_back(centerIdx); indices.push_back(p00); indices.push_back(p10); indices.push_back(p01);
            indices.push_back(centerIdx); indices.push_back(p10); indices.push_back(p11); indices.push_back(p01);
        }
    }
    return indices;
}
std::vector<uint16_t> generateSphereConeEdges(int segments, int rings) {
    std::set<std::pair<uint16_t, uint16_t>> edgeSet;
    
    const uint16_t apexIdx = 0;
    const uint16_t centerIdx = 1;
    
    auto addEdge = [&](uint16_t a, uint16_t b) {
        if (a > b) std::swap(a, b);
        edgeSet.insert({a, b});
    };
    
    // 1. 顶点到底面球面所有顶点的连线
    for (int i = 0; i <= rings; ++i) {
        for (int j = 0; j <= segments; ++j) {
            uint16_t p = 2 + i * (segments + 1) + j;
            addEdge(apexIdx, p);
        }
    }
    
    // 2. 球心到底面球面所有顶点的连线
    for (int i = 0; i <= rings; ++i) {
        for (int j = 0; j <= segments; ++j) {
            uint16_t p = 2 + i * (segments + 1) + j;
            addEdge(centerIdx, p);
        }
    }
    
    // 3. 底面球面上的经纬线
    for (int i = 0; i <= rings; ++i) {
        for (int j = 0; j <= segments; ++j) {
            uint16_t curr = 2 + i * (segments + 1) + j;
            // 经线方向 (沿 j 轴)
            if (j < segments) {
                uint16_t nextPhi = 2 + i * (segments + 1) + (j + 1);
                addEdge(curr, nextPhi);
            }
            // 纬线方向 (沿 i 轴)
            if (i < rings) {
                uint16_t nextTheta = 2 + (i + 1) * (segments + 1) + j;
                addEdge(curr, nextTheta);
            }
        }
    }
    
    // 将 set 转换为 vector 输出
    std::vector<uint16_t> edges;
    edges.reserve(edgeSet.size() * 2);
    for (const auto& edge : edgeSet) {
        edges.push_back(edge.first);
        edges.push_back(edge.second);
    }
    
    return edges;
}
SphereCone::SphereCone(/* args */){
}

SphereCone::~SphereCone(){
}

void SphereCone::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void SphereCone::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void SphereCone::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void SphereCone::Update(const void *useData){
    float theta = glm::radians(45.0f);
    int segments = 64;
    int rings = segments / 2;
    std::vector<Vertex> vertices = generateSphereConeVertices(1.0f, theta, segments, rings);
    std::vector<uint16_t> indices = generateSphereConeIndices(segments, rings);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    indices = generateSphereConeEdges(segments, rings);
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}