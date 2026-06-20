#ifndef VULKAN_POOL_H
#define VULKAN_POOL_H
#include "VulkanDevice.h"
typedef struct VULKAN_POOL{
    VkCommandPool command;
    VkDescriptorPool descriptor;
//     void Cleanup(VkDevice device);
//     void CreatePool(VulkanDevice device, uint32_t descriptorCount, VkCommandPoolCreateFlags flags = 0);

//     VkResult AllocateDescriptorSets(VkDevice device, const VkDescriptorSetLayout *setlayout, uint32_t count, VkDescriptorSet *pDescriptorSet);
};
typedef struct VULKAN_POOL VulkanPool;
VkResult vulkanAllocateCommandBuffers(VkDevice device, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandPool pool);
VkResult vulkanAllocateCommandBuffersSecondary(VkDevice device, uint32_t count, VkCommandBuffer *pCommandBuffers, VkCommandPool pool);
#endif