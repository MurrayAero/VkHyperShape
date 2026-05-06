#ifndef SPHERINDER_H
#define SPHERINDER_H
#include <vector>
#include "geometry.h"
class Spherinder:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    Spherinder(/* args */);
    ~Spherinder();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif