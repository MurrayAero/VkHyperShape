#include "vulkanPrimitive.h"
VulkanBuffer vulkanCreateIndexBuffer(const VulkanDevice *device, const void *data, VkDeviceSize size, VkQueue graphics, VkCommandPool pool){
    VulkanBuffer index = vulkanCreateBuffer(device, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    if (index.buffer != VK_NULL_HANDLE) {
        vulkanUpdateBufferData(device, data, graphics, pool, &index);
    }
    return index;
}

VulkanBuffer vulkanCreateVertexBuffer(const VulkanDevice *device, const void *data, VkDeviceSize size, VkQueue graphics, VkCommandPool pool){
    VulkanBuffer vert = vulkanCreateBuffer(device, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    if (vert.buffer != VK_NULL_HANDLE) {
        vulkanUpdateBufferData(device, data, graphics, pool, &vert);
    }
    return vert;
}

void vulkanDrawPrimitive(VkCommandBuffer command, uint32_t vertexOffset, uint32_t firstIndex, const VulkanPrimitive* primitive){
    if(primitive->index.buffer != VK_NULL_HANDLE){
        vkCmdDrawIndexed(command, primitive->indexCount, 1, firstIndex, vertexOffset, 0);
    }
    else if(primitive->vertex.buffer != VK_NULL_HANDLE){
        vkCmdDraw(command, primitive->vertexCount, 1, vertexOffset, 0);
    }
}

void vulkanBindPrimitive(VkCommandBuffer command, const VulkanPrimitive *primitive){
    VkDeviceSize offset = 0;
    if(primitive->vertex.buffer != VK_NULL_HANDLE)vkCmdBindVertexBuffers(command, 0, 1, &primitive->vertex.buffer, &offset);
    if(primitive->index.buffer != VK_NULL_HANDLE)vkCmdBindIndexBuffer(command, primitive->index.buffer, offset, primitive->indexType);
}

void vulkanDestoryPrimtive(const VulkanDevice *device, VulkanPrimitive *primitive){
    primitive->indexCount = 0;
    primitive->vertexCount = 0;
    vulkanDestroyBuffer(device->device, &primitive->index);
    vulkanDestroyBuffer(device->device, &primitive->vertex);
}
