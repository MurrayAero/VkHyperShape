#ifndef VULKANIMGUI_HPP
#define VULKANIMGUI_HPP
#include <glm/glm.hpp>

#include "../imgui/imgui.h"
#include "../vulkan/pipeline.hpp"
#include "../vulkan/primitive.hpp"
#include "../vulkan/framework.hpp"
#ifdef _MSC_VER
#undef CreateFont
#endif
namespace vulkan{
    class ImGui{
        //因为imgui确实需要一条不同的管线，所以这里需要一个管线对象
        struct{
            vk::PipelineLayout layout;
            vulkan::pipeline::Graphics pipeline;
        }pipelines;
        vulkan::Image mFont;
        vk::DescriptorSet mSet;
        vulkan::Primitive mRect;
        vk::Sampler mFontSampler;
        const vulkan::Device *mDevice;
        vk::DescriptorSetLayout mSetLayout;
        vk::DeviceSize mBufferMemoryAlignment;
        void CreatePipelineLayout(vk::Device device);
        void SetupDescriptorSetLayout(vk::Device device);
        void CreateFontsTexture(const vulkan::Device&device, vk::Queue graphics, const vulkan::Pool&pool);
        // void CreateRect(const vulkan::Device&device, vk::DeviceSize index_size, vk::DeviceSize vertex_size);
        // void CreateOrResizeBuffer(VulkanBuffer&buffer, size_t new_size, vk::BufferUsageFlagBits usage);
        void SetupRenderState(vk::CommandBuffer command, ImDrawData* draw_data, int fb_width, int fb_height);
    public:
        ImGui(/* args */);
        ~ImGui();
        void Cleanup();
        void Setup(const vulkan::Device&device, const vulkan::Pool&pool);

        void CreatePipeline(vk::Format color, vk::Format depth, vk::RenderPass renderPass, vk::PipelineCache cache = VK_NULL_HANDLE);

        void CreateFont(const char *font, vk::Queue graphics, const vulkan::Pool&pool);

        void RenderDrawData(vk::CommandBuffer command, ImDrawData* draw_data);
    };
}
#endif