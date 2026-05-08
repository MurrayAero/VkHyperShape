#include "torus.h"
void generateTorus(std::vector<Vertex>& vertices, std::vector<uint16_t>& indices, float majorRadius, float minorRadius, uint32_t radialSegments, uint32_t tubularSegments) {
    vertices.clear();
    indices.clear();

    const float TWO_PI = glm::two_pi<float>();

    for (uint32_t i = 0; i <= radialSegments; ++i) {
        float u = static_cast<float>(i) / radialSegments * TWO_PI;
        float cosU = cos(u);
        float sinU = sin(u);

        for (uint32_t j = 0; j <= tubularSegments; ++j) {
            float v = static_cast<float>(j) / tubularSegments * TWO_PI;
            float cosV = cos(v);
            float sinV = sin(v);

            float x = (majorRadius + minorRadius * cosV) * cosU;
            float y = (majorRadius + minorRadius * cosV) * sinU;
            float z = minorRadius * sinV;

            glm::vec4 pos(x, y, z, 0);
            glm::vec3 color(0.5f + 0.5f * cosU, 0.5f + 0.5f * sinU, 0.5f + 0.5f * sinV);

            vertices.emplace_back(pos, color);
        }
    }

    for (uint32_t i = 0; i < radialSegments; ++i) {
        for (uint32_t j = 0; j < tubularSegments; ++j) {
            uint16_t current = i * (tubularSegments + 1) + j;
            uint16_t next = current + (tubularSegments + 1);

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
}
Torus::Torus(/* args */){
}

Torus::~Torus(){
}

void Torus::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Torus::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Torus::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Torus::Update(const void *useData){
    const float majorRadius = 1.0f;
    const float minorRadius = 0.3f;
    const uint32_t tubularSegments = 16;
    const uint32_t radialSegments = tubularSegments * 2;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    generateTorus(vertices, indices, majorRadius, minorRadius, radialSegments, tubularSegments);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}