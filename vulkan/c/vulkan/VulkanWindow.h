// #ifndef VULKAN_WINDOW_H
// #define VULKAN_WINDOW_H
// #include "VulkanImage.h"
// #include "VulkanDevice.h"
// struct VulkanSwapchain{
//     VkExtent2D extent;
//     VkSwapchainKHR swapchain;
//     std::vector<VulkanImage>images;
//     VkSurfaceFormatKHR surfaceFormat;
//     VkResult CreateSwapchain(VulkanDevice device, VkSurfaceKHR surface);

//     void Cleanup(VkDevice device);
// };
// struct VulkanWindow{
//     VkSurfaceKHR surface;
//     VulkanImage depthImage;
//     VkRenderPass renderPass;
//     VulkanSwapchain swapchain;
//     std::vector<VkFramebuffer>framebuffers;
//     void Cleanup(VulkanDevice device);
//     void CleanupSwapchain(VkDevice device);
//     VkResult CreateRenderPass(VkDevice device, bool useDepthImage = true);
//     void CreateFrameBuffer(VulkanDevice device, bool createDepthImage = true);
// private:
//     void CreateDepthImage(VulkanDevice device, const VkExtent3D&swapchainExtent, VulkanImage&image);
// };
// #endif
