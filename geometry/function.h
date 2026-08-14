#ifndef FUNCTION_H
#define FUNCTION_H
#include "geometry.h"
#include "exprtk/exprtk_complex_adaptor.hpp"
#include "exprtk/exprtk.hpp"
#define STEP(RANGE) (0.01f * RANGE)
class Function:public Geometry{
    vulkan::Primitive mGeometry;

    cmplx::complex_t z;//, w;
    exprtk::parser<cmplx::complex_t>mParser;
    exprtk::expression<cmplx::complex_t>mExpression;
    exprtk::symbol_table<cmplx::complex_t>mSymbol_table;
    std::string generateFunctionVertices(const std::string&funStr, std::vector<Vertex>&vertices, std::vector<uint16_t>&indices, float range = 1.0f);
public:
    Function(/* args */);
    ~Function();
    virtual void Cleanup();

    virtual void Draw(vk::CommandBuffer command, vk::PipelineLayout layout);
    virtual void DrawWireframe(vk::CommandBuffer command, vk::PipelineLayout layout);

    virtual void Update(const void *useData = nullptr);
};
#endif