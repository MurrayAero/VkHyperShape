#include "cylinder.h"
void GenerateCylinder(std::vector<Vertex>& vertices, std::vector<uint16_t>& triIndices, std::vector<uint16_t>& lineIndices, float radius = .5f, float height = 5.0f, int segments = 32) {
    vertices.clear();
    triIndices.clear();
    lineIndices.clear();

    vertices.push_back(Vertex(glm::vec4(0.0f, -height / 2.0f, 0.0f, 0.0f), glm::vec3(1, 0, 0)));
    int bottomCenterIdx = 0;

    int bottomStartIdx = 1;
    for (int i = 0; i < segments; ++i) {
        float theta = glm::two_pi<float>() * i / segments;
        vertices.push_back(Vertex(glm::vec4(radius * std::cos(theta), -height / 2.0f, radius * std::sin(theta), 0.0f), glm::vec3(radius * std::cos(theta), -height / 2.0f, radius * std::sin(theta))));
    }

    int topCenterIdx = segments + 1;
    vertices.push_back(Vertex(glm::vec4(0.0f, height / 2.0f, 0.0f, 0), glm::vec3(0.0f, height / 2.0f, 0.0f)));

    int topStartIdx = segments + 2;
    for (int i = 0; i < segments; ++i) {
        float theta = glm::two_pi<float>() * i / segments;
        vertices.push_back(Vertex(glm::vec4(radius * std::cos(theta), height / 2.0f, radius * std::sin(theta), 0), glm::vec3(radius * std::cos(theta), height / 2.0f, radius * std::sin(theta))));
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        int b0 = bottomStartIdx + i;
        int b1 = bottomStartIdx + next;
        int t0 = topStartIdx + i;
        int t1 = topStartIdx + next;

        triIndices.push_back(b0); triIndices.push_back(t0); triIndices.push_back(b1);
        triIndices.push_back(b1); triIndices.push_back(t0); triIndices.push_back(t1);
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        triIndices.push_back(bottomCenterIdx); 
        triIndices.push_back(bottomStartIdx + next); 
        triIndices.push_back(bottomStartIdx + i);
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        triIndices.push_back(topCenterIdx); 
        triIndices.push_back(topStartIdx + i); 
        triIndices.push_back(topStartIdx + next);
    }

    for (int i = 0; i < segments; ++i) {
        lineIndices.push_back(bottomStartIdx + i);
        lineIndices.push_back(topStartIdx + i);
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        lineIndices.push_back(bottomStartIdx + i);
        lineIndices.push_back(bottomStartIdx + next);
    }

    for (int i = 0; i < segments; ++i) {
        int next = (i + 1) % segments;
        lineIndices.push_back(topStartIdx + i);
        lineIndices.push_back(topStartIdx + next);
    }
}
Cylinder::Cylinder(/* args */){
}

Cylinder::~Cylinder(){
}

void Cylinder::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Cylinder::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Cylinder::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Cylinder::Update(const void *useData){
    std::vector<Vertex>vertices;
    std::vector<uint16_t>indices, wireframe;
    GenerateCylinder(vertices, indices, wireframe);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, wireframe.data(), sizeof(uint16_t) * wireframe.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, wireframe.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }

}