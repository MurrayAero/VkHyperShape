#ifndef PIPELINE_H
#define PIPELINE_H
#include "geometry.h"
struct CylinderParameter{
    float radius = .015;
    float samples = 20;
    uint32_t segments = 16;
    //这样写能直接传给imgui
    std::array<float, 4>point[4] = { {1, 0, 0, 0}, { 0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
};
class Pipeline:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    Pipeline(/* args */);
    ~Pipeline();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
class Font:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    Font(/* args */);
    ~Font();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif