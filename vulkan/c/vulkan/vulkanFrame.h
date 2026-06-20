// #ifndef VULKAN_FRAME_H
// #define VULKAN_FRAME_H
// #include "VulkanPool.h"
// #include "VulkanBuffer.h"
// #include "VulkanDevice.h"
// #include "VulkanImage.h"
// #define VK_CHECK(x)                                                 \
// do{                                                               \
//         VkResult err = x;                                           \
//         if (err != VK_SUCCESS){                                       \
//                 printf("vulkan error:in function %s line %d information %s\n", __FUNCTION__, __LINE__, vulkanFrame::cvmx_chip_type_to_string(err)); \
//                 assert(0);           \
//         }                                                    \
// } while (0)
// struct VulkanQueue{
//         VkQueue video = VK_NULL_HANDLE;
//         VkQueue present = VK_NULL_HANDLE;
//         VkQueue graphics = VK_NULL_HANDLE;
//         VkQueue compute = VK_NULL_HANDLE;
//         void CreateQueue(VulkanDevice device, VkSurfaceKHR surface){
//                 int32_t queueFamilies[4];
//                 queueFamilies[3] = device.GetVideoQueueFamiliesIndex();
//                 queueFamilies[0] = device.GetQueueFamiliesIndex(surface);
//                 queueFamilies[1] = device.GetQueueFamiliesIndex(VK_QUEUE_GRAPHICS_BIT);
//                 queueFamilies[2] = device.GetQueueFamiliesIndex(VK_QUEUE_COMPUTE_BIT);
//                 if(queueFamilies[3] != -1)vkGetDeviceQueue(device.device, queueFamilies[3], 0, &video);
//                 if(queueFamilies[0] != -1)vkGetDeviceQueue(device.device, queueFamilies[0], 0, &present);
//                 if(queueFamilies[1] != -1)vkGetDeviceQueue(device.device, queueFamilies[1], 0, &graphics);
//                 if(queueFamilies[2] != -1)vkGetDeviceQueue(device.device, queueFamilies[2], 0, &compute);
//         }
// };
// struct VulkanSynchronize{
//     std::vector<VkFence>fences;
//     std::vector<VkSemaphore>imageAcquired;
//     std::vector<VkSemaphore>renderComplete;
//     void Cleanup(VkDevice device);
//     void CreateSynchronize(VkDevice device, uint32_t swapchainImageCount);
// };
// namespace vulkanFrame{
//     void BeginRenderPass(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight);
//     void BeginRenderPass(VkCommandBuffer command, VkFramebuffer frame, VkRenderPass renderpass, uint32_t windowWidth, uint32_t windowHeight, uint32_t clearValueCount, VkClearValue *pClearValues, int32_t renderAreaWidthOffset = 0, int32_t renderAreaHeightOffset = 0);

//     void BeginCommands(VkCommandBuffer command, VkCommandBufferUsageFlags flags);
//     VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VulkanPool pool);

//     const char *cvmx_chip_type_to_string(VkResult type);

//     VkResult CreateTextureSampler(VkDevice device, VkSampler &sampler);
    
//     VkResult CreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutBinding *pBinding, uint32_t count, VkDescriptorSetLayout *pSetLayout);

//     void EndSingleTimeCommands(VkDevice device, VkCommandPool pool, VkQueue graphics, VkCommandBuffer commandBuffer);

//     void InsertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);

//     // VkResult Prepare(VkDevice device, uint32_t *imageIndex, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired);
//     uint32_t Prepare(VkDevice device, VkSwapchainKHR swapchain, const VkSemaphore&imageAcquired, void(*recreateSwapchain)(void* userData), void* userData);
//     VkResult Present(uint32_t imageIndex, VkSwapchainKHR swapchain, const VkQueue present, const VkSemaphore&renderComplete, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);
//     //返回vkQueuePresentKHR的执行结果
//     VkResult Render(VkDevice device, uint32_t currentFrame, const VkCommandBuffer& commandbuffers, VkSwapchainKHR swapchain, const VulkanQueue&vulkanQueue, const VulkanSynchronize&vulkanSynchronize, void(*recreateSwapchain)(void* userData) = nullptr, void* userData = nullptr);

//     // void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
//     // void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);
//     // void SetImageLayout(VkCommandBuffer cmdbuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t imageCount);

//     VkResult Submit(const VkCommandBuffer&commandbuffers, VkQueue graphics, const VkSemaphore&imageAcquired, const VkSemaphore&renderComplete, const VkFence&fence = VK_NULL_HANDLE);
//     //setlayoutBindings可以用偏移
//     void UpdateDescriptorSets(VkDevice device, const VkDescriptorSetLayoutBinding *pBindings, uint32_t count, const std::vector<VulkanBuffer>&descriptorBuffer, const std::vector<VulkanImage>&descriptorImage, VkDescriptorSet&destSet, const VkSampler&textureSampler = VK_NULL_HANDLE);
// };
// #endif