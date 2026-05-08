#include "kleinBottle.h"
void GenerateKleinBottle(std::vector<Vertex>&vertices, std::vector<uint16_t>&indices, float radius = 1, int uSegments = 60, int vSegments = 30){
    vertices.clear();
    indices.clear();

    const float a = radius, b = radius * 0.5f;
    for (int i = 0; i <= uSegments; ++i) {
        const float u = (glm::two_pi<float>() * i) / uSegments;
        const float half_u = u / 2.0f;
        for (int j = 0; j <= vSegments; ++j) {
            float v = (glm::two_pi<float>() * j) / vSegments;

            float x = (a + b * cosf(v)) * cosf(u);
            float y = (a + b * cosf(v)) * sinf(u);
            float z = b * sinf(v) * cosf(half_u);
            float w = b * sinf(v) * sinf(half_u);

            float pseudo_normal_z = sinf(v) * cosf(half_u); 

            float color_blend = pseudo_normal_z * 0.5f + 0.5f;

            glm::vec3 color_outside = glm::vec3(1.0f, 0.1f, 0.1f);
            glm::vec3 color_inside  = glm::vec3(0.1f, 0.1f, 1.0f);

            glm::vec3 final_color = glm::mix(color_inside, color_outside, color_blend);
            
            vertices.push_back(Vertex(
                glm::vec4(x, y, z, w), 
                final_color
            ));
        }
    }

    for (int i = 0; i < uSegments; ++i) {
        for (int j = 0; j < vSegments; ++j) {
            uint16_t p00 = i * (vSegments + 1) + j;
            uint16_t p10 = (i + 1) * (vSegments + 1) + j;
            uint16_t p01 = i * (vSegments + 1) + (j + 1);
            uint16_t p11 = (i + 1) * (vSegments + 1) + (j + 1);

            indices.push_back(p00); indices.push_back(p10); indices.push_back(p01);
            indices.push_back(p10); indices.push_back(p11); indices.push_back(p01);
        }
    }
}
// void Generate(std::vector<uint16_t>&indices, std::vector<Vertex>&vertices, int uRes, int vRes) {
//     Vertex vert;
//     for (int i = 0; i <= uRes; i++) {
//         float u = glm::two_pi<float>() * i / uRes;
        
//         for (int j = 0; j <= vRes; j++) {
//             float v = glm::two_pi<float>() * j / vRes;
//             if (u < M_PI) {
//                 float scale = 4.0f * (1.0f - cos(u)/2.0f);
//                 // float w = scale * sin(v);  // v=0 时 w=0, v=π/2 时 w=+scale, v=π 时 w=0, v=3π/2 时 w=-scale
//                 vert.pos = glm::vec4(6.0f * cos(u) * (1.0f + sin(u)) + 4.0f * (1.0f - cos(u)/2.0f) * cos(u) * cos(v),
//                 16.0f * sin(u) + 4.0f * (1.0f - cos(u)/2.0f) * sin(u) * cos(v), 
//                 4.0f * (1.0f - cos(u)/2.0f) * sin(v), 0);
//                 vert.color = glm::vec3(0);
//             } else {
//                 vert.pos = glm::vec4(6.0f * cos(u) * (1.0f + sin(u)) - 4.0f * (1.0f - cos(u)/2.0f) * cos(v), 
//                 16.0f * sin(u), 
//                 4.0f * (1.0f - cos(u)/2.0f) * sin(v),
//                 4.0f * (1.0f - cos(u)/2.0f) * sin(u) * cos(v));
//                 vert.color = glm::vec3(vert.pos.w);
//             }
//             vert.pos *= 0.05f;
//             vertices.push_back(vert);
            
//             if (i < uRes && j < vRes) {
//                 int current = i * (vRes + 1) + j;
//                 int next = (i + 1) * (vRes + 1) + j;

//                 indices.push_back(current);
//                 indices.push_back(current + 1);
//                 indices.push_back(next);
                
//                 indices.push_back(current + 1);
//                 indices.push_back(next + 1);
//                 indices.push_back(next);
//             }
//         }
//     }
// }
KleinBottle::KleinBottle(/* args */){
}

KleinBottle::~KleinBottle(){
}

void KleinBottle::Cleanup(){
    mKleinBottle.Destroy(*gpu.device);
}

void KleinBottle::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mKleinBottle.Bind(command);
    mKleinBottle.Draw(command);
}

void KleinBottle::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mKleinBottle.Bind(command);
    mKleinBottle.Draw(command);
}

void KleinBottle::Update(const void *useData){
    std::vector<Vertex>vertices;
    std::vector<uint16_t>indices;
    GenerateKleinBottle(vertices, indices);
    if(!mKleinBottle.IsVaildIndex() || !mKleinBottle.IsVaildVertex()){
        mKleinBottle.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mKleinBottle.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mKleinBottle.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mKleinBottle.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}
