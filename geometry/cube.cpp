#include "cube.h"
void generateCubeVertices(std::vector<Vertex>&vertices, std::vector<uint16_t>&indices){
    vertices = {
        //正面
        Vertex(glm::vec4(-.5f, -.5f,  .5f, 0), glm::vec3(1, 0, 0)),//左下角//0
        Vertex(glm::vec4(.5f, -.5f,  .5f, 0), glm::vec3(0, 1, 0)),//右下角//1
        Vertex(glm::vec4(.5f,  .5f,  .5f, 0), glm::vec3(0, 0, 1)),//右上角//2
        Vertex(glm::vec4(-.5f,  .5f,  .5f, 0), glm::vec3(1, 1, 0)),//左上角//3
        //背面
        Vertex(glm::vec4(-.5f, -.5f, -.5f, 0), glm::vec3(1, 0, 1)),//左下角//4
        Vertex(glm::vec4(.5f, -.5f, -.5f, 0), glm::vec3(.5, .5, .5)),//右下角//5
        Vertex(glm::vec4(.5f,  .5f, -.5f, 0), glm::vec3(1, 1, 1)),//右上角//6
        Vertex(glm::vec4(-.5f,  .5f, -.5f, 0), glm::vec3(1, .5, .5)),//左上角//7
    };
    indices = {
        0,1,2, 2,3,0,//正面
        1,5,6, 6,2,1, //右面
        7,6,5, 5,4,7, //背面
        4,0,3, 3,7,4, 
        4,5,1, 1,0,4, 
        3,2,6, 6,7,3,
    };
}
auto generateCubeEdge(){
    std::vector<uint16_t> indices = {
        // 正面四条边
        0, 1,  1, 2,  2, 3,  3, 0,
        // 背面四条边
        4, 5,  5, 6,  6, 7,  7, 4,
        // 连接正背面的四条边
        0, 4,  1, 5,  2, 6,  3, 7
    };
    return indices;
}
Cube::Cube(/* args */)
{
}

Cube::~Cube()
{
}
void Cube::Cleanup(){
    mGeometry.Destroy(*gpu.device);
    mWireframe.Destroy(*gpu.device);
}

void Cube::Draw(vk::CommandBuffer command, vk::PipelineLayout layout){
    mGeometry.Bind(command);
    mGeometry.Draw(command);
}

void Cube::DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout){
    mWireframe.Bind(command);
    mWireframe.Draw(command);
}

void Cube::Update(const void *useData){
    std::vector<Vertex>vertices;
    std::vector<uint16_t>indices, wireframe;
    generateCubeVertices(vertices, indices);
    wireframe = generateCubeEdge();
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