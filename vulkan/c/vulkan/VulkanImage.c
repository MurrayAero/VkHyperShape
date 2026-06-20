#include "VulkanImage.h"
VulkanImage vulkanCrateImage(const VulkanDevice *device, const VkExtent3D *size, VkImageUsageFlags usage, VkFormat format, uint32_t arrayLayer, VkImageTiling tiling, VkSampleCountFlagBits samples){
    VulkanImage image;
    image.size = *size;
    image.tiling = tiling;
    image.channels = 4;
    image.mipLevels = 1;
    image.format = format;
    image.samples = samples;
    image.arrayLayer = arrayLayer;
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.mipLevels = image.mipLevels;
    info.extent = image.size;
    info.tiling = tiling;
    info.usage = usage;
    info.format = format;
    if(image.size.depth > 1){
        info.imageType = VK_IMAGE_TYPE_3D;
    }
    else{
        info.imageType = VK_IMAGE_TYPE_2D;
    }
    info.samples = samples;
    info.initialLayout = image.layout;
    info.arrayLayers = arrayLayer;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image.layout = VK_IMAGE_LAYOUT_UNDEFINED;
    if(arrayLayer == 6 && size->width == size->height){
        info.flags =  VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }
    VmaAllocationCreateInfo allocCI = {};
    if (tiling == VK_IMAGE_TILING_LINEAR) {
        allocCI.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    } 
    else{
        allocCI.usage = VMA_MEMORY_USAGE_AUTO;
    }
    VmaAllocationInfo allocationInfo;
    vmaCreateImage(device->allocator, &info, &allocationInfo, &image.image, &image.allocation, VK_NULL_HANDLE);
    return image;
}
void vulkanCopyImage(VkCommandBuffer command, VulkanImage *src, VulkanImage *dst){
    const uint32_t layerCount = dst->subresourceRange.layerCount;
    SetLayout(command, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT, src);
    SetLayout(command, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, dst);

    VkImageCopy *regions = malloc(sizeof(VkImageCopy) * src->mipLevels);
    for (uint32_t i = 0; i < src->mipLevels; ++i){
        VkImageCopy *r = &regions[i];

        r->srcSubresource.aspectMask = dst->subresourceRange.aspectMask;
        r->srcSubresource.mipLevel = i;
        r->srcSubresource.baseArrayLayer = dst->subresourceRange.baseArrayLayer;
        r->srcSubresource.layerCount = layerCount;

        r->dstSubresource.aspectMask = dst->subresourceRange.aspectMask;
        r->dstSubresource.mipLevel = i;
        r->dstSubresource.baseArrayLayer = dst->subresourceRange.baseArrayLayer;
        r->dstSubresource.layerCount = layerCount;

        r->extent.width = MAX(dst->size.width >> i, 1u);
        r->extent.height = MAX(dst->size.height >> i, 1u);
        r->extent.depth = 1u; // 2D array / cube 都是 1
    }
    vkCopyImage(src->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions);

    SetLayout(command, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM, VK_ACCESS_SHADER_READ_BIT, src);

    SetLayout(command, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM, VK_ACCESS_SHADER_READ_BIT, dst);

    free(regions);
}

void vulkanCopyDataFromBuffer(VkCommandBuffer command, const VulkanBuffer *src, uint32_t imageCount, VulkanImage *image){
    const VkDeviceSize imageSize = image->size.width * image->size.height * image->channels;
    SetImageLayout(command, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, image);
	VkBufferImageCopy *bufferCopyRegions = malloc(sizeof(VkBufferImageCopy) * imageCount);
	for (size_t i = 0; i < imageCount; ++i) {
        bufferCopyRegions[i].imageExtent = image->size;
        bufferCopyRegions[i].bufferOffset = imageSize * i;
        bufferCopyRegions[i].imageSubresource.layerCount = 1;
        bufferCopyRegions[i].imageSubresource.baseArrayLayer = i;
        bufferCopyRegions[i].imageSubresource.aspectMask = image->subresourceRange.aspectMask;
	}
	vkCmdCopyBufferToImage(command, src->buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageCount, &bufferCopyRegions);
    SetImageLayout(command, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, image);
    free(bufferCopyRegions);
}
VkImageView vulkanCreateImageView(VkDevice device, VkImageAspectFlags aspectMask, VkImageViewType type, VulkanImage *image){
    image->subresourceRange.baseMipLevel = 0;
    image->subresourceRange.baseArrayLayer = 0;
    image->subresourceRange.aspectMask = aspectMask;
    image->subresourceRange.levelCount = image->mipLevels;
    image->subresourceRange.layerCount = image->arrayLayer;
    VkImageView view;
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = image;
    info.format = image->format;
    info.viewType = type;
    info.subresourceRange = image->subresourceRange;
    vkCreateImageView(device, &info, VK_NULL_HANDLE, &view);
    return view;
}
void vulkanDestroyImage(const VulkanDevice *device, VulkanImage *image){
    if(image->image != VK_NULL_HANDLE && image->allocation != VK_NULL_HANDLE)vmaDestroyImage(device->allocator, image->image, image->allocation);
    if(image->view != VK_NULL_HANDLE)vkDestroyImageView(device, image->view, VK_NULL_HANDLE);
    image->view = VK_NULL_HANDLE;
    image->image = VK_NULL_HANDLE;
    image->allocation = VK_NULL_HANDLE;
}

void vulkanSetImageLayout(VkCommandBuffer command, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VulkanImage *image){
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = image->layout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = image->subresourceRange;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;

    vkCmdPipelineBarrier(command, srcStage, dstStage, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &barrier);

    image->layout = newLayout;
}

void vulkanSetLayout(VkCommandBuffer command, VkImageLayout newLayout, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess, VulkanImage *image){
    VkPipelineStageFlags srcStage;
    VkAccessFlags        srcAccess;
    if (image->layout == newLayout) return;

    if (image->layout == VK_IMAGE_LAYOUT_UNDEFINED){
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        srcAccess = VK_ACCESS_NONE;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_GENERAL){
        srcStage  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        srcAccess = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL){
        srcStage  = VK_PIPELINE_STAGE_TRANSFER_BIT;
        srcAccess = VK_ACCESS_TRANSFER_READ_BIT;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        srcStage  = VK_PIPELINE_STAGE_TRANSFER_BIT;
        srcAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
        srcStage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        srcAccess = VK_ACCESS_SHADER_READ_BIT;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL){
        srcStage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        srcAccess = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL){
        srcStage  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        srcAccess = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    else if(image->layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL){
        srcStage  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        srcAccess = VK_ACCESS_NONE;
    }
    else if (image->layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR){
        srcStage  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        srcAccess = VK_ACCESS_NONE;
    }
    SetLayout(command, newLayout, srcStage, dstStage, srcAccess, dstAccess, image);
}

void vulkanUpdateImageData(const VulkanDevice *device, const void *datas, VkQueue graphics, VkCommandPool pool, VulkanImage *image){
	VkDeviceSize imageSize = image->size.width * image->size.height * image->channels;
    VulkanBuffer temp = vulkanCreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    vulkanUpdateBufferData(device, imageSize, datas, pool, &temp);
	VkCommandBuffer command = vulkanBeginSingleTimeCommands(device->device, pool);
    vulkanCopyDataFromBuffer(command, &temp, 1, image);
	vulkanFrameworkEndSingleTimeCommands(device->device, pool, graphics, command);
    vulkanDestroyBuffer(device, &temp);
}

void vulkanUpdateImageArrayData(const VulkanDevice *device, const void *const *datas, uint32_t imageCount, VkQueue graphics, VkCommandPool pool, VulkanImage *image){
	const VkDeviceSize imageSize = image->size.width * image->size.height * image->channels;
	VulkanBuffer temp = vulkanCreateBuffer(device, imageSize * imageCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    for (size_t i = 0; i < imageCount; i++){
        vulkanUpdateBufferDataFromHost(device, datas, i * imageSize, &temp);
    }
	VkCommandBuffer command = vulkanBeginSingleTimeCommands(device->device, pool);
    CopyImage(command, temp, imageCount);
	EndSingleTimeCommands(device->device, pool, graphics, command);
    vulkanDestroyBuffer(device, &temp);
}

void vulkanUpdateImageDataFromHost(const VulkanDevice *device, const void *datas, uint32_t imageCount, VulkanImage *image){
    void *data;
    const uint32_t imageSize = image->size.width * image->size.height * image->channels;
    vmaMapMemory(device->allocator, image->allocation, &data);
    memory(data, datas, imageSize);
    vmaUnmapMemory(device->allocator, image->allocation);
}