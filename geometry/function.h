#ifndef FUNCTION_H
#define FUNCTION_H
#include "geometry.h"
#define STEP(RANGE) (0.01f * RANGE)
class Function:public Geometry{
    vulkan::Primitive mGeometry;
public:
    Function(/* args */);
    ~Function();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif