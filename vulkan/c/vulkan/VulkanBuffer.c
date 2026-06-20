#include "VulkanBuffer.h"
VulkanBuffer vulkanCreateBuffer(const VulkanDevice *device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage){
    VulkanBuffer buffer = {};
    buffer.size = size;
    buffer.count = 1;

    VkBufferCreateInfo bufInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufInfo.size  = size;
    bufInfo.usage = usage;
    VmaAllocationCreateInfo allocInfo = {};
    if(usage&VK_BUFFER_USAGE_TRANSFER_SRC_BIT){
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    }
    else{
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    }
    // VmaAllocationInfo allocationInfo = {};
    // // allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
    // //                 | VMA_ALLOCATION_CREATE_MAPPED_BIT; 
    // vmaCreateBuffer(device->allocator, &bufInfo, &allocInfo, &buffer, &buffer.allocation, &allocationInfo);
        // if(keepMapped)buffer.pMappedData = allocationInfo.pMappedData;
    vmaCreateBuffer(device->allocator, &bufInfo, &allocInfo, &buffer.buffer, &buffer.allocation, VK_NULL_HANDLE);
    return buffer;
}

VulkanBuffer vulkanCreateBufferDynamic(VulkanDevice device, VkDeviceSize minUniformBufferOffset, uint32_t count, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage){
    VulkanBuffer buffer = vulkanCreateBuffer(&device, minUniformBufferOffset * count, usage, memoryUsage);
    buffer.count = count;
    buffer.size = minUniformBufferOffset;
    return buffer;
}
void vulkanUpdateBufferData(const VulkanDevice *device, const void *pData, VkQueue graphics, VkCommandPool pool, VulkanBuffer *buffer){
    if(!pData || buffer->buffer == VK_NULL_HANDLE)return;
    VulkanBuffer temporary = vulkanCreateBuffer(device, buffer->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    // temporary.CreateBuffer(device, size, , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // temporary.UpdateData(device.device, size, pData);
    VkCommandBuffer command;
    VkBufferCopy regions = {};
    regions.size = buffer->size;
    vulkanAllocateCommandBuffers(device->device, 1, &command, pool);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(command, &beginInfo);

    vkCmdCopyBuffer(command, temporary.buffer, buffer, 1, &regions);

	vkEndCommandBuffer(command);
    
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command;
	VK_CHECK(vkQueueSubmit(graphics, 1, &submitInfo, VK_NULL_HANDLE));
    
	vkFreeCommandBuffers(device->device, pool, 1, &command);

    vulkanDestroyBuffer(device, &temporary);
}

void vulkanUpdateBufferDataFromHost(const VulkanDevice *device, const void *pData, VkDeviceSize offset, VulkanBuffer *buffer){
    if(!pData || buffer->buffer == VK_NULL_HANDLE)return;
	void* data;
	vmaMapMemory(device->allocator, buffer->allocation, &data);
	memcpy(data + offset, pData, buffer->size);
	vmaUnmapMemory(device->allocator, buffer->allocation);
}

void vulkanDestroyBuffer(const VulkanDevice *device, VulkanBuffer *buffer){
    if(buffer->allocation != VK_NULL_HANDLE)vmaDestroyBuffer(device->allocator, buffer->buffer, buffer->allocation);
    buffer = VK_NULL_HANDLE;
    buffer->allocation = VK_NULL_HANDLE;
}