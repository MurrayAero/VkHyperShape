#ifndef VULKAN_IMAGE_H
#define VULKAN_IMAGE_H
#include <string.h>
#include "VulkanBuffer.h"
#include "vulkanFramework.h"
#define MAX(NUMBER1, NUMBER2)((NUMBER1) > (NUMBER2))
//AMD Ryzen 7 5800H还有华硕天选2的3060提示不支持"VK_FORMAT_R8G8B8_UNORM和VK_IMAGE_TYPE_2D以及一些其他格式一起用"
struct VULKAN_IMAGE{
    VkImage image;
    VkExtent3D size;
    VkFormat format;
    uint32_t channels;
    VkImageView view;
    uint32_t mipLevels;
    uint32_t arrayLayer;
    VkImageTiling tiling;
    VkImageLayout layout;
    VmaAllocation allocation;
    VkSampleCountFlagBits samples;
    VkImageSubresourceRange subresourceRange;
};
typedef struct VULKAN_IMAGE VulkanImage;

VulkanImage vulkanCrateImage(const VulkanDevice *device, const VkExtent3D *size, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageTiling tiling, VkSampleCountFlagBits samples);

void vulkanCopyImage(VkCommandBuffer command, VulkanImage *src, VulkanImage *dst);
void vulkanCopyDataFromBuffer(VkCommandBuffer command, const VulkanBuffer *src, uint32_t imageCount, VulkanImage *image);

//VK_IMAGE_VIEW_TYPE_2D
VkImageView vulkanCreateImageView(VkDevice device, VkImageAspectFlags aspectMask, VkImageViewType type, VulkanImage *image);

void vulkanDestroyImage(const VulkanDevice *device, VulkanImage *image);
void vulkanSetLayout(VkCommandBuffer command, VkImageLayout newLayout, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess, VulkanImage *image);
void vulkanSetImageLayout(VkCommandBuffer command, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VulkanImage *image);

void vulkanUpdateImageData(const VulkanDevice *device, const void *datas, VkQueue graphics, VkCommandPool pool, VulkanImage *image);
void vulkanUpdateImageArrayData(const VulkanDevice *device, const void*const*datas, uint32_t imageCount, VkQueue graphics, VkCommandPool pool, VulkanImage *image);

void vulkanUpdateImageDataFromHost(const VulkanDevice *device, const void *datas, uint32_t imageCount, VulkanImage *image);
#endif