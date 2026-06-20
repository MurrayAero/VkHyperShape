#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H
#include "VulkanPool.h"
#include "VulkanDevice.h"
#ifndef VK_CHECK
#define VK_CHECK(x)                                                 \
do{                                                               \
        VkResult err = x;                                           \
        if (err != VK_SUCCESS){                                       \
                printf("vulkan error:in function %s line %d information %s\n", __FUNCTION__, __LINE__, cvmx_chip_type_to_string(err)); \
                assert(0);           \
        }                                                    \
} while (0)
#endif
struct VULKAN_BUFFER{
    uint32_t count;
    VkBuffer buffer;
    VkDeviceSize size;
    void *pMappedData;
    VmaAllocation allocation;
};

typedef struct VULKAN_BUFFER VulkanBuffer;

VulkanBuffer vulkanCreateBuffer(const VulkanDevice *device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
VulkanBuffer vulkanCreateBufferDynamic(VulkanDevice device, VkDeviceSize minUniformBufferOffset, uint32_t count, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

void vulkanDestroyBuffer(const VulkanDevice *device, VulkanBuffer *buffer);

void vulkanUpdateBufferData(const VulkanDevice *device, const void *pData, VkQueue graphics, VkCommandPool pool, VulkanBuffer *buffer);

void vulkanUpdateBufferDataFromHost(const VulkanDevice *device, const void * pData, VkDeviceSize offset, VulkanBuffer *buffer);
#endif