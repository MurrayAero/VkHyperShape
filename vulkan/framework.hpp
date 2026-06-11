#ifndef VULKAN_FRAME_H
#define VULKAN_FRAME_H
#include <assert.h>
#include "image.hpp"
#include "queue.hpp"
#define VK_CHECK_LOG(LOGGER, x)                                                 \
do{                                                               \
        vk::Result err = x;                                           \
        if (err != vk::Result::eSuccess){                                       \
                LOGGER->error("vulkan error:in function {}, line {}, information {}", __FUNCTION__, __LINE__, cvmx_chip_type_to_string(err)); \
                spdlog::default_logger()->flush();\
                spdlog::shutdown();\
                assert(0);           \
        }                                                    \
} while (0)
#define VK_CHECK(x)                                                 \
do{                                                               \
        vk::Result err = x;                                           \
        if (err != vk::Result::eSuccess){                                       \
                printf("vulkan error:in function %s line %d information %s\n", __FUNCTION__, __LINE__, cvmx_chip_type_to_string(err)); \
                assert(0);           \
        }                                                    \
} while (0)

const char *cvmx_chip_type_to_string(vk::Result type);
namespace vulkan{
        namespace framework{
                void BeginCommands(vk::CommandBuffer command, vk::CommandBufferUsageFlags flags, vk::CommandBufferInheritanceInfo*inheritanceInfo = nullptr);
                void BeginRenderPass(vk::CommandBuffer command, vk::Framebuffer frame, vk::RenderPass renderPass, uint32_t windowWidth, uint32_t windowHeight);
                void BeginRendering(vk::CommandBuffer command, const std::vector<Image*>&color, Image&depth, uint32_t windowWidth, uint32_t windowHeight, vk::RenderingFlags flags = {});

                vk::CommandBuffer BeginSingleTimeCommands(vk::Device device, const Pool&pool);

                vk::Sampler CreateTextureSampler(vk::Device device, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat, vk::Filter filter = vk::Filter::eLinear);

                void EndRendering(vk::CommandBuffer command, Image&color);
                void EndRendering(vk::CommandBuffer command, const std::vector<Image*>&color);
                void EndSingleTimeCommands(vk::CommandBuffer command, vk::Queue queue, vk::Device device, const Pool&pool);

		void UpdateDescriptorSets(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding> &bindings, const std::vector<vulkan::Buffer> &buffer, const std::vector<vulkan::Image> &image, vk::DescriptorSet &destSet, const vk::Sampler &sampler = {}, bool shaderReadOnlyImageLayout = true);
        };
}
#endif