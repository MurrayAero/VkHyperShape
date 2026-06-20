#include "buffer.hpp"
#include "framework.hpp"
namespace vulkan{
    vk::CommandBuffer Buffer::BeginSingleTimeCommands(const Device&device, const Pool&pool){
        auto command = pool.AllocateCommandBuffers(device);
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        command.begin(beginInfo);
        return command;
    }
    void Buffer::EndSingleTimeCommands(vk::CommandBuffer command, vk::Queue queue, const Device&device, const Pool&pool){
        command.end();
        
        vk::SubmitInfo submitInfo = {};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command;

        auto result = queue.submit(1, &submitInfo, VK_NULL_HANDLE);
        VK_CHECK(result);
        VK_CHECK_LOG(result);
        queue.waitIdle();

        pool.FreeCommandBuffers(device, command);
    }
    Buffer::Buffer(){
    }
    Buffer::~Buffer(){
    }
    void Buffer::Create(const Device&device, vk::DeviceSize size, vk::BufferUsageFlags usage, vma::MemoryUsage memoryUsage, bool keepMapped){
        if(buffer != VK_NULL_HANDLE)Destroy(device);
        // printf("in function %s: buffer %p, size %d, usage %d, memory usage\n", __FUNCTION__, buffer, size, usage, memoryUsage);
        this->size = size;
        vk::BufferCreateInfo info{};
        info.size  = size;
        info.usage = usage;
        info.sharingMode = vk::SharingMode::eExclusive;
        vma::AllocationCreateInfo allocCI{};
        allocCI.usage = memoryUsage;
        if (keepMapped) {
            allocCI.flags = vma::AllocationCreateFlagBits::eMapped | vma::AllocationCreateFlagBits::eHostAccessSequentialWrite;
        }
        vma::AllocationInfo allocationInfo;
        vma::Allocator allocator = device.GetAllocator();
        std::tie(buffer, allocation) = allocator.createBuffer(info, allocCI, allocationInfo);
        if(keepMapped)pMappedData = allocationInfo.pMappedData;
    }
    void Buffer::Create(const Device&device, vk::DeviceSize minUniformBufferOffset, uint32_t count, vk::BufferUsageFlags usage, vma::MemoryUsage memoryUsage, bool keepMapped){
        this->count = count;
        Create(device, minUniformBufferOffset * count, usage, memoryUsage, keepMapped);
        size = minUniformBufferOffset;
    }
    void Buffer::Destroy(const Device&device){
        // printf("in function %s: buffer %p, size %d\n", __FUNCTION__, buffer, size);
        vma::Allocator allocator = device.GetAllocator();
        allocator.destroyBuffer(buffer, allocation);
        buffer = VK_NULL_HANDLE;
        allocation = VK_NULL_HANDLE;
    //     device.destroyBuffer(buffer);
    //     device.freeMemory(memory);
    //     buffer = VK_NULL_HANDLE;
    //     memory= VK_NULL_HANDLE;
    }
    void Buffer::PipelineBarrier(vk::CommandBuffer command, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask){\
        vk::BufferMemoryBarrier barrier{};
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = buffer;
        barrier.offset = 0;
        barrier.size = size;
        command.pipelineBarrier(srcStageMask, dstStageMask, {}, {}, {barrier}, {});
    }
    // void Buffer::Create(const Device&device, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties){
    //     if(buffer != VK_NULL_HANDLE)Destroy(device.device);
    //     this->size = size;
    //     vk::BufferCreateInfo info = {};
    //     info.sType = vk::StructureType::eBufferCreateInfo;
    //     info.size = size;
    //     info.usage = usage;
    //     info.sharingMode = vk::SharingMode::eExclusive;
    //     buffer = device.device.createBuffer(info);
    // 	vk::MemoryRequirements memoryRequirements = device.device.getBufferMemoryRequirements(buffer);
    //     uint32_t memoryTypeIndex = device.findMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties);
    //     memory = device.device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, memoryTypeIndex));
    // 	device.device.bindBufferMemory(buffer, memory, 0);
    // }
    // void Buffer::Create(const Device&device, vk::DeviceSize minUniformBufferOffset, uint32_t count, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties){
    //     this->count = count;
    //     CreateBuffer(device, minUniformBufferOffset * count, usage, properties);
    //     size = minUniformBufferOffset;
    // }
    void Buffer::UpdateData(const Device&device, const void *pData, vk::Queue queue, const Pool&pool){
        if(!pData || buffer == VK_NULL_HANDLE)return;
        Buffer temporary;
        temporary.Create(device, size, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuToGpu);
        // temporary.CreateBuffer(device, size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent);
        temporary.UpdateData(device, size, pData);

        UpdateData(device, temporary, queue, pool);

        temporary.Destroy(device);
    }
    void Buffer::UpdateData(const Device &device, const vulkan::Buffer &buffer, vk::Queue queue, const Pool &pool){
        auto command = BeginSingleTimeCommands(device, pool);

        vk::BufferCopy regions = {};
        regions.size = size;
        command.copyBuffer(buffer, this->buffer, 1, &regions);

        EndSingleTimeCommands(command, queue, device, pool);
    }
    void Buffer::UpdateData(const Device &device, const void *pData, vk::DeviceSize offset){
        UpdateData(device, size, pData, offset);
    }
    void Buffer::UpdateData(const Device&device, vk::DeviceSize size, const void *pData, vk::DeviceSize offset){
        vma::Allocator allocator = device.GetAllocator();
        if(pMappedData){
            std::memcpy((char *)pMappedData + offset, pData, size);
        }
        else{
            void *data = allocator.mapMemory(allocation);
            std::memcpy((char *)data + offset, pData, size);
            allocator.unmapMemory(allocation);    
        }
    }
    // void Buffer::UpdateData(vk::Device device, const void *pData, vk::DeviceSize offset){
    //     UpdateData(device, size, pData, offset);
    // }
    // void Buffer::UpdateData(vk::Device device, vk::DeviceSize size, const void *pData, vk::DeviceSize offset){
    //     if(!pData || buffer == VK_NULL_HANDLE)return;
    // #ifdef USE_VULKAN_MEMORY_ALLOCATOR
    //     device.allocator.mapMemory(allocation, pData);
    //     allocator.unmapMemory(allocation);
    // #else
    // 	void* data = device.mapMemory(memory, offset, size);
    // 	memcpy(data, pData, size);
    // 	device.unmapMemory(memory);
    // #endif
    // }
}