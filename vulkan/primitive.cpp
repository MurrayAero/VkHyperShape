#include "primitive.hpp"
namespace vulkan{
    void Primitive::CreateIndexBuffer(const Device&device, const void *data, vk::DeviceSize size, vk::Queue graphics, const Pool&pool, vk::IndexType indexType){
        CreateIndexBuffer(device, size, false, indexType);
        if(data)index.UpdateData(device, data, graphics, pool);
    }

    void Primitive::CreateVertexBuffer(const Device&device, vk::DeviceSize size, uint32_t vertexCount, bool hostvisible){
        if(vertexCount)this->vertexCount = vertexCount;
        if(hostvisible){
            vertex.Create(device, size, vk::BufferUsageFlagBits::eVertexBuffer, vma::MemoryUsage::eCpuToGpu);
        }
        else{
            vertex.Create(device, size, vk::BufferUsageFlagBits::eVertexBuffer|vk::BufferUsageFlagBits::eTransferDst);
        }
    }

    void Primitive::CreateIndexBuffer(const Device&device, vk::DeviceSize size, bool hostvisible, vk::IndexType indexType){
        this->indexType = indexType;
        indexCount = static_cast<uint32_t>(indexType == vk::IndexType::eUint16?size / sizeof(uint16_t):size / sizeof(uint32_t));
        if(hostvisible){
            index.Create(device, size, vk::BufferUsageFlagBits::eIndexBuffer, vma::MemoryUsage::eCpuToGpu);
        }
        else{
            index.Create(device, size, vk::BufferUsageFlagBits::eIndexBuffer|vk::BufferUsageFlagBits::eTransferDst);
        }
    }

    void Primitive::CreateVertexBuffer(const Device&device, const void *data, vk::DeviceSize size, uint32_t vertexCount, vk::Queue graphics, const Pool&pool){
        CreateVertexBuffer(device, size, vertexCount, false);
        if(data)vertex.UpdateData(device, data, graphics, pool);
    }

    void Primitive::Bind(vk::CommandBuffer command) const noexcept{
        vk::DeviceSize offset = 0;
        if(vertex.GetBuffer() != VK_NULL_HANDLE)command.bindVertexBuffers(0, 1, &vertex.GetBuffer(), &offset);
        if(index.GetBuffer() != VK_NULL_HANDLE)command.bindIndexBuffer(index.GetBuffer(), offset, indexType);
    }

    void Primitive::Destroy(const Device&device){
        index.Destroy(device);
        vertex.Destroy(device);
        vertexCount = 0;
        indexCount = 0;
    }
    void Primitive::Draw(vk::CommandBuffer command, uint32_t vertexOffset, uint32_t firstIndex) const noexcept{
        if(index.GetBuffer() != VK_NULL_HANDLE){
            command.drawIndexed(indexCount, 1, firstIndex, vertexOffset, 0);
        }
        else if(vertex.GetBuffer() != VK_NULL_HANDLE){
            command.draw(vertexCount, 1, vertexOffset, 0);
        }
    }

    void Primitive::FlushMappedMemoryRanges(const Device&device)const{
        // vk::MappedMemoryRange range[2] = {};
        // range[0].sType = vk::StructureType::eMappedMemoryRange;
        // range[0].memory = vertex.memory;
        // range[0].size = VK_WHOLE_SIZE;
        // range[1].sType = vk::StructureType::eMappedMemoryRange;
        // range[1].memory = index.memory;
        // range[1].size = VK_WHOLE_SIZE;
        // return device.flushMappedMemoryRanges(2, range);
        index.FlushMappedMemoryRanges(device);
        vertex.FlushMappedMemoryRanges(device);
    }
}