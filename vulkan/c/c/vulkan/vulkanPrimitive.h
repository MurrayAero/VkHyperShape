#ifndef VULKAN_PRIMITIVE_H
#define VULKAN_PRIMITIVE_H
#include "VulkanBuffer.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
typedef struct VULKAN_PRIMITIVE{
    VulkanBuffer index;
    VulkanBuffer vertex;
    uint32_t indexCount;
    uint32_t vertexCount;
    VkIndexType indexType;

};
typedef struct VULKAN_PRIMITIVE VulkanPrimitive;

void vulkanBindPrimitive(VkCommandBuffer command, const VulkanPrimitive* primitive);

VulkanBuffer vulkanCreateIndexBuffer(const VulkanDevice *device, const void* data, VkDeviceSize size, VkQueue graphics, VkCommandPool pool);
VulkanBuffer vulkanCreateVertexBuffer(const VulkanDevice *device, const void* data, VkDeviceSize size, VkQueue graphics, VkCommandPool pool);

void vulkanDrawPrimitive(VkCommandBuffer command, uint32_t vertexOffset, uint32_t firstIndex, const VulkanPrimitive* primitive);

void vulkanDestoryPrimtive(const VulkanDevice *device, VulkanPrimitive *primitive);
#endif