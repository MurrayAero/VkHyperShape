#ifndef HOPF_H
#define HOPF_H
#include "geometry.h"
const float CLIFFORD_R = 0.70710678f; // 1/sqrt(2)
class Hopf:public Geometry{
    vulkan::Primitive mGeometry;
    vulkan::Primitive mWireframe;
public:
    Hopf(/* args */);
    ~Hopf();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif