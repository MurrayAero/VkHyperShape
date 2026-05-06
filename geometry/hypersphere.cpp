#include "hypersphere.h"
std::vector<Vertex>generateHypersphere(uint32_t stacksPhi, uint32_t stacksTheta, uint32_t slicesPsi){
    std::vector<Vertex> vertices;
    vertices.reserve(stacksPhi * stacksTheta * slicesPsi);
    /*
        x = R * sin(φ) * sin(θ) * cos(ψ)
        y = R * sin(φ) * sin(θ) * sin(ψ)
        z = R * sin(φ) * cos(θ)
        w = R * cos(φ)
    */
    auto colorFromPos = [](const glm::vec4& p) -> glm::vec3 {
        return glm::normalize(glm::vec3(p.x, p.y, p.z)) * 0.5f + 0.5f;
    };
    for (uint32_t i = 0; i <= stacksPhi; ++i) {
        float phi = glm::pi<float>() * i / stacksPhi;

        for (uint32_t j = 0; j <= stacksTheta; ++j) {
            float theta = glm::pi<float>() * j / stacksTheta;

            for (uint32_t k = 0; k < slicesPsi; ++k) {
                float psi = glm::two_pi<float>() * k / slicesPsi;

                float sinPhi = sin(phi);
                float x = sinPhi * sin(theta) * cos(psi);
                float y = sinPhi * sin(theta) * sin(psi);
                float z = sinPhi * cos(theta);
                float w = cos(phi);

                glm::vec4 pos(x, y, z, w);
                glm::vec3 color = colorFromPos(pos);

                vertices.emplace_back(pos, color);
            }
        }
    }
    return vertices;
}
std::vector<uint16_t>generateHypersphereIndices(uint32_t stacksPhi, uint32_t stacksTheta, uint32_t slicesPsi){
    std::vector<uint16_t> indices;
    auto idx = [&](uint16_t i, uint16_t j, uint16_t k) {
        return i * (stacksTheta + 1) * slicesPsi + j * slicesPsi + k;
    };
    auto addTetra = [&](uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
        indices.push_back(d);
    };

    for (uint16_t i = 0; i < stacksPhi; ++i) {
        for (uint16_t j = 0; j < stacksTheta; ++j) {
            for (uint16_t k = 0; k < slicesPsi; ++k) {

                uint16_t a = idx(i, j, k);
                uint16_t b = idx(i + 1, j, k);
                uint16_t c = idx(i, j + 1, k);
                uint16_t d = idx(i, j, (k + 1) % slicesPsi);
                uint16_t e = idx(i + 1, j + 1, k);
                uint16_t f = idx(i + 1, j, (k + 1) % slicesPsi);
                uint16_t g = idx(i, j + 1, (k + 1) % slicesPsi);
                uint16_t h = idx(i + 1, j + 1, (k + 1) % slicesPsi);

                addTetra(a, b, c, d);
                addTetra(b, c, d, e);
                addTetra(b, d, e, f);
                addTetra(c, d, e, g);
                addTetra(d, e, f, g);
                addTetra(e, f, g, h);
            }
        }
    }
    return indices;
}
std::vector<uint16_t>generateHypersphereWireframe(uint32_t stacksPhi, uint32_t stacksTheta, uint32_t slicesPsi){
    std::vector<uint16_t> indices;
    auto idx = [&](uint16_t i, uint16_t j, uint16_t k) {
        return i * (stacksTheta + 1) * slicesPsi + j * slicesPsi + k;
    };

    auto addEdge = [&](uint32_t a, uint32_t b) {
        indices.push_back(a);
        indices.push_back(b);
    };

    for (uint32_t i = 0; i <= stacksPhi; ++i) {
        for (uint32_t j = 0; j <= stacksTheta; ++j) {
            for (uint32_t k = 0; k < slicesPsi; ++k) {

                uint32_t cur = idx(i, j, k);

                if (i < stacksPhi)
                    addEdge(cur, idx(i + 1, j, k));

                if (j < stacksTheta)
                    addEdge(cur, idx(i, j + 1, k));

                addEdge(cur, idx(i, j, (k + 1) % slicesPsi));
            }
        }
    }
    return indices;
}
Hypersphere::Hypersphere(/* args */){
}

Hypersphere::~Hypersphere(){
}

void Hypersphere::Cleanup(){
    mWireframe.Destroy(*gpu.device);
    mHypersphere.Destroy(*gpu.device);
}

void Hypersphere::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mHypersphere.Bind(command);
    mHypersphere.Draw(command);
}

void Hypersphere::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Hypersphere::Update(const void *useData){
    const uint32_t stacksPhi = 16;
    const uint32_t stacksTheta = stacksPhi;
    const uint32_t slicesPsi = stacksPhi * 2;
    std::vector<Vertex>vertices = generateHypersphere(stacksPhi, stacksTheta, slicesPsi);
    std::vector<uint16_t> indices = generateHypersphereIndices(stacksPhi, stacksTheta, slicesPsi);
    if(!mHypersphere.IsVaildIndex() || !mHypersphere.IsVaildVertex()){
        mHypersphere.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mHypersphere.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mHypersphere.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mHypersphere.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    indices = generateHypersphereWireframe(stacksPhi, stacksTheta, slicesPsi);
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}

std::vector<Vertex>generateSphere(uint32_t stackCount, uint32_t sectorCount){
    const float radius = 1.0f;
    std::vector<Vertex> vertices;
    vertices.reserve((stackCount + 1) * (sectorCount + 1));

    for (uint32_t i = 0; i <= stackCount; ++i) {
        float phi = M_PI * i / stackCount;// [0, π]
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);

        for (uint32_t j = 0; j <= sectorCount; ++j) {
            float theta = 2.0f * M_PI * j / sectorCount;// [0, 2π]
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            float x = radius * sinPhi * cosTheta;
            float y = radius * sinPhi * sinTheta;
            float z = radius * cosPhi;

            glm::vec4 pos(x, y, z, 0);
            glm::vec3 color(pos);

            vertices.emplace_back(pos, color);
        }
    }
    return vertices;
}
std::vector<uint16_t>generateSphereIndices(uint32_t stackCount, uint32_t sectorCount){
    std::vector<uint16_t> indices;

    auto idx = [&](uint16_t stack, uint16_t sector) {
        return stack * (sectorCount + 1) + sector;
    };

    for (uint16_t i = 0; i < stackCount; ++i) {
        for (uint16_t j = 0; j < sectorCount; ++j) {

            uint16_t a = idx(i, j);
            uint16_t b = idx(i + 1, j);
            uint16_t c = idx(i, j + 1);
            uint16_t d = idx(i + 1, j + 1);

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(b);
            indices.push_back(d);
            indices.push_back(c);
        }
    }
    return indices;
}
std::vector<uint16_t>generateSphereWireframe(uint32_t stackCount, uint32_t sectorCount){
    std::vector<uint16_t> indices;

    auto idx = [&](uint16_t stack, uint16_t sector) {
        return stack * (sectorCount + 1) + sector;
    };

    for (uint16_t i = 0; i < stackCount; ++i) {
        for (uint16_t j = 0; j < sectorCount; ++j) {

            uint16_t a = idx(i, j);
            uint16_t b = idx(i + 1, j);
            uint16_t c = idx(i, j + 1);
            uint16_t d = idx(i + 1, j + 1);

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            indices.push_back(b);
            indices.push_back(d);
            indices.push_back(c);
        }
    }
    return indices;
}
Sphere::Sphere(/* args */){
}

Sphere::~Sphere(){
}

void Sphere::Cleanup(){
    mWireframe.Destroy(*gpu.device);
    mSphere.Destroy(*gpu.device);
}

void Sphere::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mSphere.Bind(command);
    mSphere.Draw(command);
}

void Sphere::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Sphere::Update(const void *useData){
    const uint32_t stackCount = 36;
    const uint32_t sectorCount = stackCount * 2;
    std::vector<Vertex>vertices = generateSphere(stackCount, sectorCount);
    std::vector<uint16_t> indices = generateSphereIndices(stackCount, sectorCount);
    if(!mSphere.IsVaildIndex() || !mSphere.IsVaildVertex()){
        mSphere.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mSphere.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mSphere.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mSphere.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    indices = generateSphereWireframe(stackCount, sectorCount);
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}