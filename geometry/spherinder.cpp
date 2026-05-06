#include "spherinder.h"
std::vector<Vertex> GenerateSpherinderVertices(uint32_t stacksCount, uint32_t slicesCount) {
    std::vector<Vertex> vertices;
    auto addSphere = [&](float w) {
        // 北极
        glm::vec4 nPos(0.0f, 0.0f, 1.0f, w);
        vertices.emplace_back(nPos, glm::vec3(0.5f + 0.5f * nPos.x, 0.5f + 0.5f * nPos.y, 0.5f + 0.5f * nPos.z));

        // 中间层 rings: i = 1 .. stacksCount-1
        for (uint32_t i = 1; i < stacksCount; ++i) {
            float phi = M_PI * static_cast<float>(i) / static_cast<float>(stacksCount);
            float cosPhi = std::cos(phi);
            float sinPhi = std::sin(phi);
            for (uint32_t j = 0; j < slicesCount; ++j) {
                float theta = glm::two_pi<float>() * static_cast<float>(j) / static_cast<float>(slicesCount);
                float x = sinPhi * std::cos(theta);
                float y = sinPhi * std::sin(theta);
                float z = cosPhi;
                glm::vec4 pos(x, y, z, w);
                glm::vec3 color(0.5f + 0.5f * x, 0.5f + 0.5f * y,0.5f + 0.5f * z);
                vertices.emplace_back(pos, color);
            }
        }

        glm::vec4 sPos(0.0f, 0.0f, -1.0f, w);
        vertices.emplace_back(sPos, glm::vec3(0.5f + 0.5f * sPos.x, 0.5f + 0.5f * sPos.y, 0.5f + 0.5f * sPos.z));
    };

    addSphere(-1.0f);
    addSphere(1.0f);
    return vertices;
}

std::vector<uint16_t> GenerateSpherinderIndices(uint32_t stacksCount, uint32_t slicesCount) {
    std::vector<uint16_t> indices;
    uint32_t vertsPerSphere = 2 + (stacksCount - 1) * slicesCount;

    uint16_t bottomNorth = 0;
    uint16_t bottomSouth = static_cast<uint16_t>(vertsPerSphere - 1);
    uint16_t topOffset   = static_cast<uint16_t>(vertsPerSphere);
    uint16_t topNorth    = topOffset;
    uint16_t topSouth    = static_cast<uint16_t>(topOffset + vertsPerSphere - 1);

    auto bottomIdx = [&](uint32_t i, uint32_t j) -> uint16_t {
        // i: [1, stacksCount-1], j 自动取模
        return static_cast<uint16_t>(1 + (i - 1) * slicesCount + (j % slicesCount));
    };
    auto topIdx = [&](uint32_t i, uint32_t j) -> uint16_t {
        return static_cast<uint16_t>(topOffset + 1 + (i - 1) * slicesCount + (j % slicesCount));
    };

    // ---------- 底部球面 ----------
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(bottomNorth);
        indices.push_back(bottomIdx(1, j));
        indices.push_back(bottomIdx(1, j + 1));
    }
    for (uint32_t i = 1; i < stacksCount - 1; ++i) {
        for (uint32_t j = 0; j < slicesCount; ++j) {
            uint16_t a = bottomIdx(i, j);
            uint16_t b = bottomIdx(i, j + 1);
            uint16_t c = bottomIdx(i + 1, j);
            uint16_t d = bottomIdx(i + 1, j + 1);
            indices.push_back(a); indices.push_back(b); indices.push_back(d);
            indices.push_back(a); indices.push_back(d); indices.push_back(c);
        }
    }
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(bottomSouth);
        indices.push_back(bottomIdx(stacksCount - 1, j + 1));
        indices.push_back(bottomIdx(stacksCount - 1, j));
    }

    // ---------- 顶部球面 ----------
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(topNorth);
        indices.push_back(topIdx(1, j));
        indices.push_back(topIdx(1, j + 1));
    }
    for (uint32_t i = 1; i < stacksCount - 1; ++i) {
        for (uint32_t j = 0; j < slicesCount; ++j) {
            uint16_t a = topIdx(i, j);
            uint16_t b = topIdx(i, j + 1);
            uint16_t c = topIdx(i + 1, j);
            uint16_t d = topIdx(i + 1, j + 1);
            indices.push_back(a); indices.push_back(b); indices.push_back(d);
            indices.push_back(a); indices.push_back(d); indices.push_back(c);
        }
    }
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(topSouth);
        indices.push_back(topIdx(stacksCount - 1, j + 1));
        indices.push_back(topIdx(stacksCount - 1, j));
    }

    // ---------- 侧面 (S² × I 的柱面壁) ----------
    // 北极附近
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(bottomNorth);
        indices.push_back(topIdx(1, j));
        indices.push_back(topIdx(1, j + 1));
    }
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(topNorth);
        indices.push_back(bottomIdx(1, j + 1));
        indices.push_back(bottomIdx(1, j));
    }

    // 中间各层环
    for (uint32_t i = 1; i < stacksCount; ++i) {
        for (uint32_t j = 0; j < slicesCount; ++j) {
            uint16_t a = bottomIdx(i, j);
            uint16_t b = bottomIdx(i, j + 1);
            uint16_t c = topIdx(i, j);
            uint16_t d = topIdx(i, j + 1);
            indices.push_back(a); indices.push_back(b); indices.push_back(c);
            indices.push_back(b); indices.push_back(d); indices.push_back(c);
        }
    }

    // 南极附近
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(bottomSouth);
        indices.push_back(topIdx(stacksCount - 1, j));
        indices.push_back(topIdx(stacksCount - 1, j + 1));
    }
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(topSouth);
        indices.push_back(bottomIdx(stacksCount - 1, j + 1));
        indices.push_back(bottomIdx(stacksCount - 1, j));
    }

    return indices;
}

std::vector<uint16_t> GenerateSpherinderWireframe(uint32_t stacksCount, uint32_t slicesCount) {
    std::vector<uint16_t> indices;
    uint32_t vertsPerSphere = 2 + (stacksCount - 1) * slicesCount;
    uint16_t bottomNorth = 0;
    uint16_t bottomSouth = static_cast<uint16_t>(vertsPerSphere - 1);
    uint16_t topOffset   = static_cast<uint16_t>(vertsPerSphere);
    uint16_t topNorth    = topOffset;
    uint16_t topSouth    = static_cast<uint16_t>(topOffset + vertsPerSphere - 1);

    auto bottomIdx = [&](uint32_t i, uint32_t j) -> uint16_t {
        return static_cast<uint16_t>(1 + (i - 1) * slicesCount + (j % slicesCount));
    };
    auto topIdx = [&](uint32_t i, uint32_t j) -> uint16_t {
        return static_cast<uint16_t>(topOffset + 1 + (i - 1) * slicesCount + (j % slicesCount));
    };

    // 底部球面：经线
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(bottomNorth);
        indices.push_back(bottomIdx(1, j));
        for (uint32_t i = 1; i < stacksCount - 1; ++i) {
            indices.push_back(bottomIdx(i, j));
            indices.push_back(bottomIdx(i + 1, j));
        }
        indices.push_back(bottomIdx(stacksCount - 1, j));
        indices.push_back(bottomSouth);
    }
    // 底部球面：纬线
    for (uint32_t i = 1; i < stacksCount; ++i) {
        for (uint32_t j = 0; j < slicesCount; ++j) {
            indices.push_back(bottomIdx(i, j));
            indices.push_back(bottomIdx(i, j + 1));
        }
    }

    // 顶部球面：经线
    for (uint32_t j = 0; j < slicesCount; ++j) {
        indices.push_back(topNorth);
        indices.push_back(topIdx(1, j));
        for (uint32_t i = 1; i < stacksCount - 1; ++i) {
            indices.push_back(topIdx(i, j));
            indices.push_back(topIdx(i + 1, j));
        }
        indices.push_back(topIdx(stacksCount - 1, j));
        indices.push_back(topSouth);
    }
    // 顶部球面：纬线
    for (uint32_t i = 1; i < stacksCount; ++i) {
        for (uint32_t j = 0; j < slicesCount; ++j) {
            indices.push_back(topIdx(i, j));
            indices.push_back(topIdx(i, j + 1));
        }
    }

    // 侧面：沿 w 轴连接对应顶点
    indices.push_back(bottomNorth);
    indices.push_back(topNorth);
    for (uint32_t i = 1; i < stacksCount; ++i) {
        for (uint32_t j = 0; j < slicesCount; ++j) {
            indices.push_back(bottomIdx(i, j));
            indices.push_back(topIdx(i, j));
        }
    }
    indices.push_back(bottomSouth);
    indices.push_back(topSouth);

    return indices;
}
Spherinder::Spherinder(/* args */){
}

Spherinder::~Spherinder(){
}

void Spherinder::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Spherinder::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Spherinder::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Spherinder::Update(const void *useData){
    const uint32_t stacksCount = 36;
    const uint32_t slicesCount = stacksCount * 2;
    std::vector<Vertex> vertices = GenerateSpherinderVertices(stacksCount, slicesCount);
    std::vector<uint16_t> indices = GenerateSpherinderIndices(stacksCount, slicesCount);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
    indices = GenerateSpherinderWireframe(stacksCount, slicesCount);
    if(!mWireframe.IsVaildIndex() || !mWireframe.IsVaildVertex()){
        mWireframe.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mWireframe.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mWireframe.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mWireframe.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}