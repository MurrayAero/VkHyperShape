#ifndef VULKAN_WINDOW_HPP
#define VULKAN_WINDOW_HPP
#include "swapchain.hpp"
#include "framework.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#define DEPTH_FORMAT vk::Format::eD32SfloatS8Uint
namespace vulkan{
    struct Synchronize{
        vk::Fence fences;
        vk::Semaphore imageAcquired;
        vk::Semaphore renderComplete;
        void Create(vk::Device device);
        void Destroy(vk::Device device);
    };
    class RenderEngine{
        Image depthStencil;
        Swapchain swapchain;
        vk::SurfaceKHR surface;
        uint32_t currentFrame = 0;
        vk::RenderPass renderPass;
        const Device *mDevice;
        std::vector<vk::Framebuffer>framebuffers;

        std::vector<vk::CommandPool>commandPool;
        std::vector<std::vector<vk::CommandBuffer>>secondaryCommandBuffers;

        uint32_t threadCount = 0;
        void(*threadFun)(vk::CommandBuffer, uint32_t);

        std::vector<Synchronize>synchronize;
        std::vector<vk::CommandBuffer>commandBuffers;

        void CreateRenderPass(vk::Device device);

        void CreateSynchronize(vk::Device device);
    public:
        RenderEngine();
        ~RenderEngine();

        void Cleanup();

        void CleanupSwapchain();
    
        void CreateDepthImage();
        void CreateFrameBuffer();
        void CreateSwapchain(uint32_t width, uint32_t height);

        vk::Result Present(uint32_t imageIndex, const vk::Queue present, const vk::Semaphore&renderComplete);
        //应该在vulkanwindow类重建交换链;继续提供创建交换链函数指针是因为目前还没有想好怎么把glfwwindow*或相关变量传给RecreateSwapchain函数
        void Render(vk::Device device, const Queue&vulkanQueue, void(*recordCommand)(vk::CommandBuffer, vk::Framebuffer , vk::RenderPass), void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);
        void DynamicRendering(vk::Device device, const Queue&vulkanQueue, void(*recordCommand)(vk::CommandBuffer, vulkan::Image&, vulkan::Image&), void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);

        void Setup(const Device&device, uint32_t width, uint32_t height, const vulkan::Pool&pool);
        void SetThreadFun(void(*threadFun)(vk::CommandBuffer, uint32_t), uint32_t threadCount)noexcept;
        void Submit(const vk::CommandBuffer&commandbuffers, vk::Queue graphics, const vk::Semaphore&imageAcquired, const vk::Semaphore&renderComplete, const vk::Fence&fence = VK_NULL_HANDLE);
        inline auto&GetSurface()const noexcept{
            return surface;
        }
        inline auto&GetSwapchain()const noexcept{
            return swapchain;
        }
        inline auto&GetRenderPass()const noexcept{
            return renderPass;
        }
        inline auto&GetDepthImage()noexcept{
            return depthStencil;
        }
        inline auto&GetSwapchainSize()const noexcept{
            return swapchain.GetExtent();
        }
        inline auto&GetSurfaceFormat()const noexcept{
            return swapchain.GetSurfaceFormat();
        }
        inline uint32_t GetCurrentFrame()const noexcept{
            return currentFrame;
        }
        inline auto&GetSecondyCommandBuffers()const noexcept{
            // std::vector<vk::CommandBuffer>commandbuffer;
            // for(uint32_t i = 0; i < threadCount; ++i){
            //     commandbuffer.push_back(secondaryCommandBuffers[i][currentFrame]);
            // }
            return secondaryCommandBuffers[currentFrame];
        }
        inline auto&GetFrameBuffer(uint32_t index)const noexcept{
            return index < framebuffers.size()?framebuffers[index]:framebuffers[0];
        }
        inline void SetSurface(vk::SurfaceKHR surface){
            this->surface = surface;
        }
    };
}
#endif