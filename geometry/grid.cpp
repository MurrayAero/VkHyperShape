#include "grid.h"
void Grid3D::Cleanup(){
    mGrid.Destroy(*gpu.device);
}

void Grid3D::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGrid.Bind(command);
    mGrid.Draw(command);
}

void Grid3D::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGrid.Bind(command);
    mGrid.Draw(command);
}

void Grid3D::Update(const void *useData){
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    std::array<glm::vec4, 8> pos = {
        glm::vec4(-1.0f, -1.0f, -1.0f, 0.0f),  // 0: -x, -y, -z
        glm::vec4(-1.0f, -1.0f,  1.0f, 0.0f),  // 1: -x, -y, +z
        glm::vec4(-1.0f,  1.0f, -1.0f, 0.0f),  // 2: -x, +y, -z
        glm::vec4(-1.0f,  1.0f,  1.0f, 0.0f),  // 3: -x, +y, +z
        glm::vec4( 1.0f, -1.0f, -1.0f, 0.0f),  // 4: +x, -y, -z
        glm::vec4( 1.0f, -1.0f,  1.0f, 0.0f),  // 5: +x, -y, +z
        glm::vec4( 1.0f,  1.0f, -1.0f, 0.0f),  // 6: +x, +y, -z
        glm::vec4( 1.0f,  1.0f,  1.0f, 0.0f)   // 7: +x, +y, +z
    };

    const float offset = 0.25f;
    const int grid_lines = static_cast<int>(1.0f / offset + 0.5f) + 1;
    const int grid_cells = grid_lines - 1;

    auto trilinear_interp = [&](float x, float y, float z) -> glm::vec4 {
        glm::vec4 x0 = glm::mix(pos[0], pos[4], x);  // y=-1, z=-1
        glm::vec4 x1 = glm::mix(pos[1], pos[5], x);  // y=-1, z=+1
        glm::vec4 x2 = glm::mix(pos[2], pos[6], x);  // y=+1, z=-1
        glm::vec4 x3 = glm::mix(pos[3], pos[7], x);  // y=+1, z=+1
        
        glm::vec4 y0 = glm::mix(x0, x2, y);  // z=-1
        glm::vec4 y1 = glm::mix(x1, x3, y);  // z=+1
        
        return glm::mix(y0, y1, z);
    };

    std::map<std::tuple<int, int, int>, int> vertex_map;
    auto get_or_create_vertex = [&](int ix, int iy, int iz) -> int {
        auto key = std::make_tuple(ix, iy, iz);
        auto it = vertex_map.find(key);
        if (it != vertex_map.end()) return it->second;
        
        float x = ix * offset;
        float y = iy * offset;
        float z = iz * offset;
        
        auto point = trilinear_interp(x, y, z);
        int idx = vertices.size();
        const glm::vec4 pos = glm::vec4(point.x, point.y, point.z, point.w);
        vertices.push_back(Vertex(pos, pos));
        vertex_map[key] = idx;
        return idx;
    };
    for (int iz = 0; iz < grid_lines; ++iz) {
        for (int iy = 0; iy < grid_lines; ++iy) {
            for (int ix = 0; ix < grid_cells; ++ix) {
                int i0 = get_or_create_vertex(ix, iy, iz);
                int i1 = get_or_create_vertex(ix + 1, iy, iz);
                indices.push_back(i0);
                indices.push_back(i1);
            }
        }
    }

    // 2. Y方向线：固定 x,z，变化 y（垂直线）
    for (int iz = 0; iz < grid_lines; ++iz) {
        for (int ix = 0; ix < grid_lines; ++ix) {
            for (int iy = 0; iy < grid_cells; ++iy) {
                int i0 = get_or_create_vertex(ix, iy, iz);
                int i1 = get_or_create_vertex(ix, iy + 1, iz);
                indices.push_back(i0);
                indices.push_back(i1);
            }
        }
    }

    // 3. Z方向线：固定 x,y，变化 z（深度线）
    for (int iy = 0; iy < grid_lines; ++iy) {
        for (int ix = 0; ix < grid_lines; ++ix) {
            for (int iz = 0; iz < grid_cells; ++iz) {
                int i0 = get_or_create_vertex(ix, iy, iz);
                int i1 = get_or_create_vertex(ix, iy, iz + 1);
                indices.push_back(i0);
                indices.push_back(i1);
            }
        }
    }
    if(!mGrid.IsVaildIndex() || !mGrid.IsVaildVertex()){
        mGrid.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGrid.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGrid.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGrid.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}

void Grid4D::Cleanup(){
    mGrid.Destroy(*gpu.device);
}

void Grid4D::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGrid.Bind(command);
    mGrid.Draw(command);
}

void Grid4D::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGrid.Bind(command);
    mGrid.Draw(command);
}

void Grid4D::Update(const void *useData){
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    const float offset = 0.25f;
    const int grid_lines = static_cast<int>(1.0f / offset + 0.5f) + 1;

    std::array<glm::vec4, 16> pos = {
        glm::vec4(-1.0f, -1.0f, -1.0f, -1.0f),  // 0
        glm::vec4(-1.0f, -1.0f,  1.0f, -1.0f),  // 1
        glm::vec4(-1.0f,  1.0f, -1.0f, -1.0f),  // 2
        glm::vec4(-1.0f,  1.0f,  1.0f, -1.0f),  // 3
        glm::vec4( 1.0f, -1.0f, -1.0f, -1.0f),  // 4
        glm::vec4( 1.0f, -1.0f,  1.0f, -1.0f),  // 5
        glm::vec4( 1.0f,  1.0f, -1.0f, -1.0f),  // 6
        glm::vec4( 1.0f,  1.0f,  1.0f, -1.0f),  // 7
        glm::vec4(-1.0f, -1.0f, -1.0f,  1.0f),  // 8
        glm::vec4(-1.0f, -1.0f,  1.0f,  1.0f),  // 9
        glm::vec4(-1.0f,  1.0f, -1.0f,  1.0f),  // 10
        glm::vec4(-1.0f,  1.0f,  1.0f,  1.0f),  // 11
        glm::vec4( 1.0f, -1.0f, -1.0f,  1.0f),  // 12
        glm::vec4( 1.0f, -1.0f,  1.0f,  1.0f),  // 13
        glm::vec4( 1.0f,  1.0f, -1.0f,  1.0f),  // 14
        glm::vec4( 1.0f,  1.0f,  1.0f,  1.0f)   // 15
    };
    auto interpolate = [&](float x, float y, float z, float w) -> glm::vec4 {
        glm::vec4 x0 = glm::mix(pos[0], pos[4], x);
        glm::vec4 x1 = glm::mix(pos[1], pos[5], x);
        glm::vec4 x2 = glm::mix(pos[2], pos[6], x);
        glm::vec4 x3 = glm::mix(pos[3], pos[7], x);
        glm::vec4 x4 = glm::mix(pos[8], pos[12], x);
        glm::vec4 x5 = glm::mix(pos[9], pos[13], x);
        glm::vec4 x6 = glm::mix(pos[10], pos[14], x);
        glm::vec4 x7 = glm::mix(pos[11], pos[15], x);
        
        glm::vec4 y0 = glm::mix(x0, x2, y);
        glm::vec4 y1 = glm::mix(x1, x3, y);
        glm::vec4 y2 = glm::mix(x4, x6, y);
        glm::vec4 y3 = glm::mix(x5, x7, y);
        
        glm::vec4 z0 = glm::mix(y0, y1, z);
        glm::vec4 z1 = glm::mix(y2, y3, z);
        
        return glm::mix(z0, z1, w);
    };
    std::map<std::tuple<int,int,int,int>, int> vertex_map;
    auto get_or_create_vertex = [&](int ix, int iy, int iz, int iw) -> int {
        auto key = std::make_tuple(ix, iy, iz, iw);
        auto it = vertex_map.find(key);
        if (it != vertex_map.end()) return it->second;
        
        float x = ix * offset;
        float y = iy * offset;
        float z = iz * offset;
        float w = iw * offset;
        
        auto point = interpolate(x, y, z, w);
        int idx = vertices.size();
        const glm::vec4 pos = glm::vec4(point.x, point.y, point.z, point.w);
        vertices.push_back(Vertex(pos, pos * .5f + .5f));
        vertex_map[key] = idx;
        return idx;
    };

    // 1. X方向线：对于每个(y,z,w)组合，画一条沿X的线
    for (int iw = 0; iw < grid_lines; ++iw) {
        for (int iz = 0; iz < grid_lines; ++iz) {
            for (int iy = 0; iy < grid_lines; ++iy) {
                for (int ix = 0; ix < grid_lines - 1; ++ix) {
                    int i0 = get_or_create_vertex(ix, iy, iz, iw);
                    int i1 = get_or_create_vertex(ix + 1, iy, iz, iw);
                    indices.push_back(i0);
                    indices.push_back(i1);
                }
            }
        }
    }

    // 2. Y方向线：对于每个(x,z,w)组合，画一条沿Y的线
    for (int iw = 0; iw < grid_lines; ++iw) {
        for (int iz = 0; iz < grid_lines; ++iz) {
            for (int ix = 0; ix < grid_lines; ++ix) {
                for (int iy = 0; iy < grid_lines - 1; ++iy) {
                    int i0 = get_or_create_vertex(ix, iy, iz, iw);
                    int i1 = get_or_create_vertex(ix, iy + 1, iz, iw);
                    indices.push_back(i0);
                    indices.push_back(i1);
                }
            }
        }
    }

    // 3. Z方向线：对于每个(x,y,w)组合，画一条沿Z的线
    for (int iw = 0; iw < grid_lines; ++iw) {
        for (int iy = 0; iy < grid_lines; ++iy) {
            for (int ix = 0; ix < grid_lines; ++ix) {
                for (int iz = 0; iz < grid_lines - 1; ++iz) {
                    int i0 = get_or_create_vertex(ix, iy, iz, iw);
                    int i1 = get_or_create_vertex(ix, iy, iz + 1, iw);
                    indices.push_back(i0);
                    indices.push_back(i1);
                }
            }
        }
    }

    for (int iz = 0; iz < grid_lines; ++iz) {
        for (int iy = 0; iy < grid_lines; ++iy) {
            for (int ix = 0; ix < grid_lines; ++ix) {
                for (int iw = 0; iw < grid_lines - 1; ++iw) {
                    int i0 = get_or_create_vertex(ix, iy, iz, iw);
                    int i1 = get_or_create_vertex(ix, iy, iz, iw + 1);
                    indices.push_back(i0);
                    indices.push_back(i1);
                }
            }
        }
    }
    if(!mGrid.IsVaildIndex() || !mGrid.IsVaildVertex()){
        mGrid.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGrid.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGrid.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGrid.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}