#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <array>

#include "../mglm.hpp"
#include "../transform.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../vulkan/pipeline.hpp"
#include "../vulkan/primitive.hpp"
#include "../vulkan/framework.hpp"
//该宏作用:如果val比alignment小，则返回alignment，否则如果val大于alignment但小于alignment*2则返回alignment*2以此类推
#define ALIGN(val, alignment)((val + alignment - 1) & ~(alignment - 1))
struct UseData{
    float radius = .015;
    float samples = 20;
    float cliffordTime = 0;
    int32_t twistLoops = 1;
    //这样写能直接传给imgui
    std::array<float, 4>point[4] = { {1, 0, 0, 0}, { 0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
};
struct Vertex {
    glm::vec4 pos;
    glm::vec3 color;
    Vertex(){
        
    }
    Vertex(const glm::vec4&pos, const glm::vec3&color){
        this->pos = pos;
        this->color = color;
    }
    static vk::VertexInputBindingDescription inputBindingDescription(uint32_t binding) {
        return vk::VertexInputBindingDescription({ binding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
    }
    static vk::VertexInputAttributeDescription inputAttributeDescription(uint32_t location, vk::Format format, uint32_t offset, uint32_t binding = 0){
        vk::VertexInputAttributeDescription info;
        info.offset = offset;
        info.format = format;
        info.binding = binding;
        info.location = location;
        return info;
    }
    static std::array<vk::VertexInputAttributeDescription, 2> inputAttributeDescriptions(uint32_t binding) {
        return { inputAttributeDescription(0, vk::Format::eR32G32B32A32Sfloat, 0, binding), inputAttributeDescription(1, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color), binding) };
    }
};
class Geometry{
protected:
    struct{
        vk::Queue graphics;
        const vulkan::Pool *pool;
        const vulkan::Device *device;
    }gpu;
    // vk::DescriptorSet mSet;
public:
    Geometry(/* args */);
    ~Geometry();
    virtual void Cleanup() = 0;

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout) = 0;
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout) = 0;

    virtual void Update(const void *useData = nullptr) = 0;
    
    virtual void Setup(const vulkan::Device&device, vk::Queue graphics, const vulkan::Pool&pool);
};
#endif