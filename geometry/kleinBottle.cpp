#include "kleinBottle.h"
void GenerateKleinBottle(std::vector<Vertex>&vertices, std::vector<uint16_t>&indices, uint32_t twistLoops, uint32_t uSegments, uint32_t vSegments, float radius = 1.0f){
    vertices.clear();
    indices.clear();
    /*
        ​(a+bcos v cos u/2​)cos u
        (a+b cos v cos u/2​)sin u
        b sin v cos u / 2
        b sin v sin u / 2
    */
    const float a = radius, b = radius * 0.5f;
    for (int i = 0; i <= uSegments; ++i) {
        const float u = (glm::two_pi<float>() * i) / uSegments;
        const float half_u = u / 2.0f;
        for (int j = 0; j <= vSegments; ++j) {
            float v = (glm::two_pi<float>() * j) / vSegments;

            float x = (a + b * std::cos(v)) * std::cos(u);
            float y = (a + b * std::cos(v)) * std::sin(u);
            float z = b * std::sin(v) * std::cos(half_u * twistLoops);
            float w = b * std::sin(v) * std::sin(half_u * twistLoops);

            float color_blend = (w / b) * 0.5f + 0.5f; 
            glm::vec3 color_side_A = glm::vec3(1.0f, 0.2f, 0.2f);
            glm::vec3 color_side_B = glm::vec3(0.2f, 0.2f, 1.0f);
            glm::vec3 final_color = glm::mix(color_side_B, color_side_A, color_blend);
            
            vertices.push_back(Vertex(glm::vec4(x, y, z, w),final_color));
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
    UseData *pUseData = (UseData*)useData;
    uint32_t vSegments = 30;
    uint32_t uSegments = vSegments * 2;
    GenerateKleinBottle(vertices, indices, pUseData->twistLoops, uSegments, vSegments);
    if(!mKleinBottle.IsVaildIndex() || !mKleinBottle.IsVaildVertex()){
        mKleinBottle.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mKleinBottle.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mKleinBottle.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mKleinBottle.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}
