#ifndef CYLINDER_H
#define CYLINDER_H
#include "geometry.h"
class Cylinder:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    Cylinder(/* args */);
    ~Cylinder();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif