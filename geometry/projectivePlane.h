#ifndef PROJECTIVE_PLANE_H
#define PROJECTIVE_PLANE_H
#include "geometry.h"
class ProjectivePlane:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    ProjectivePlane(/* args */);
    ~ProjectivePlane();

    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif