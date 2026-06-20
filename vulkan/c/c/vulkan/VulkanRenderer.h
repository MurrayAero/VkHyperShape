#ifndef VULKAN_WINDOW_H
#define VULKAN_WINDOW_H
#include "VulkanImage.h"
#include "VulkanDevice.h"
struct VULKAN_SWAPCHAIN{
    VkExtent2D size;
    uint32_t imageCount;
    VulkanImage *images;
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR surfaceFormat;
};
struct VULKAN_WINDOW{
    VkSurfaceKHR surface;
    VulkanImage depthImage;
    VkRenderPass renderPass;
    VulkanSwapchain swapchain;
//     std::vector<VkFramebuffer>framebuffers;
//     void Cleanup(VulkanDevice device);
//     void CleanupSwapchain(VkDevice device);
//     VkResult CreateRenderPass(VkDevice device, bool useDepthImage = true);
//     void CreateFrameBuffer(VulkanDevice device, bool createDepthImage = true);
// private:
//     void CreateDepthImage(VulkanDevice device, const VkExtent3D&swapchainExtent, VulkanImage&image);
};
typedef struct VULKAN_WINDOW VulkanWindow;
typedef struct VULKAN_SWAPCHAIN VulkanSwapchain;

void vulkanCleanupSwapchain(const VulkanDevice *device, VulkanSwapchain *swapchain);
VulkanSwapchain vulkanCreateSwapchain(const VulkanDevice *device, VkSurfaceKHR surface);
#endif