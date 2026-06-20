#include "function.h"
void generateFunctionVertices(const std::function<std::complex<float>(const std::complex<float> &)>&fun, std::vector<Vertex>&vertices, std::vector<uint16_t>&indices, float range = 1.0f){
    const float step = STEP(range);
    const float range_min = range * -1.0f;
    const uint32_t COUNT = static_cast<uint32_t>((range - range_min) / step) + 1;
    vertices.reserve(COUNT * COUNT);
    indices.reserve((COUNT - 1) * (COUNT * 2 + 1));                   
    for (uint32_t xi = 0; xi < COUNT; ++xi) {
        float r = range_min + xi * step;                                       
        for (uint32_t yi = 0; yi < COUNT; ++yi) {
            float i_coord = range_min + yi * step;
            
            std::complex<float> z(r, i_coord), val = fun(z);
            Vertex v;
            v.pos = glm::vec4(z.real(), z.imag(), val.real(), val.imag());
            v.color = v.pos * .5f + .5f;
            vertices.push_back(v);
        }
    }
    for (uint32_t xi = 0; xi < COUNT - 1; ++xi) {
        for (uint32_t yi = 0; yi < COUNT - 1; ++yi) {
            uint32_t tl = xi * COUNT + yi;
            uint32_t tr = xi * COUNT + yi + 1;
            uint32_t bl = (xi + 1) * COUNT + yi;
            uint32_t br = (xi + 1) * COUNT + yi + 1;

            indices.push_back(tl);
            indices.push_back(tr);
            indices.push_back(bl);
 
            indices.push_back(tr);
            indices.push_back(br);
            indices.push_back(bl);
        }
    }
}
Function::Function(/* args */){
}

Function::~Function(){
}
void Function::Cleanup(){
    mGeometry.Destroy(*gpu.device);
}

void Function::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Function::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Function::Update(const void *useData){
    std::vector<Vertex>vertices;
    std::vector<uint16_t>indices;
    const UseData *parameter = (const UseData*)useData;
    generateFunctionVertices(parameter->function.fun, vertices, indices, parameter->function.range);
    // generateFunctionVertices([](const std::complex<float>&z){return std::tan(z);}, vertices, indices);
    if(!mGeometry.IsVaildIndex() || !mGeometry.IsVaildVertex()){
        mGeometry.CreateIndexBuffer(*gpu.device, indices.data(), sizeof(uint16_t) * indices.size(), gpu.graphics, *gpu.pool);
        mGeometry.CreateVertexBuffer(*gpu.device, vertices.data(), sizeof(Vertex) * vertices.size(), vertices.size(), gpu.graphics, *gpu.pool);
    }
    else{
        mGeometry.UpdateIndexData(*gpu.device, indices.data(), gpu.graphics, *gpu.pool);
        mGeometry.UpdateVertexData(*gpu.device, vertices.data(), gpu.graphics, *gpu.pool);
    }
}