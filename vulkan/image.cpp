#include "image.hpp"
namespace vulkan{
    void Image::Create(const Device&device, vk::ImageUsageFlags usage){
        if(image != VK_NULL_HANDLE)return;
        vk::ImageCreateInfo info = {};
        info.sType = vk::StructureType::eImageCreateInfo;
        info.mipLevels = mipLevels;
        info.extent = size;
        if(arrayLayer == 6 && size.width == size.height){
            info.flags = vk::ImageCreateFlagBits::eCubeCompatible;
        }
        info.tiling = tiling;
        info.usage = usage;
        info.format = format;
        if(size.depth > 1){
            info.imageType = vk::ImageType::e3D;
        }
        else{
            info.imageType = vk::ImageType::e2D;
        }
        info.samples = samples;
        info.initialLayout = layout;
        info.arrayLayers = arrayLayer;
        info.sharingMode = vk::SharingMode::eExclusive;
        // image = device.createImage(info);
        vma::AllocationCreateInfo allocCI{};
        if (tiling == vk::ImageTiling::eLinear) {
            allocCI.usage = vma::MemoryUsage::eCpuToGpu;
        } 
        else{
            allocCI.usage = vma::MemoryUsage::eAuto;
        }
        vma::AllocationInfo allocationInfo;
        vma::Allocator allocator = device.GetAllocator();
        std::tie(image, allocation) = allocator.createImage(info, allocCI, allocationInfo);
        // printf("in function %s: image %p, width %d, height %d\n", __FUNCTION__, image, size.width, size.height);
    }
    void Image::Create(const Device&device, const vk::Extent3D&extent, vk::ImageUsageFlags usage, vk::Format format){
        Create(device, extent, usage, format, 1);
    }
    void Image::Create(const Device&device, const vk::Extent3D &extent, vk::ImageUsageFlags usage, vk::Format format, uint32_t arrayLayer, vk::ImageTiling tiling, vk::SampleCountFlagBits samples){
        if(image != VK_NULL_HANDLE && (size.width != extent.width || size.height != extent.height || size.depth != extent.depth))Destroy(device);
        SetSize(extent);
        Settiling(tiling);
        SetFormt(format);
        SetSamples(samples);
        SetArrayLayer(arrayLayer);
        //if(!(usage & eTransferDst) || eTransferSrc)就用host visible
        Create(device, usage);
    }
    void Image::CreateView(vk::Device device, vk::ImageViewType type) {
        vk::ImageViewCreateInfo info = {};
        vk::ImageSubresourceRange subresourceRange = {};
        subresourceRange.levelCount = mipLevels;
        subresourceRange.layerCount = arrayLayer;
        subresourceRange.aspectMask = aspectMask;
        info.sType = vk::StructureType::eImageViewCreateInfo;
        info.image = image;
        info.format = format;
        info.viewType = type;
        info.subresourceRange = subresourceRange;
        view = device.createImageView(info);
    }
    void Image::SetLayout(vk::CommandBuffer command, vk::ImageLayout newLayout, vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess)noexcept{
        vk::PipelineStageFlags srcStage;
        vk::AccessFlags        srcAccess;
        if (layout == newLayout) return;

        if (layout == vk::ImageLayout::eUndefined){
            srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
            srcAccess = vk::AccessFlagBits::eNone;
        }
        else if (layout == vk::ImageLayout::eGeneral){
            srcStage  = vk::PipelineStageFlagBits::eAllCommands;
            srcAccess = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        }
        else if (layout == vk::ImageLayout::eTransferSrcOptimal){
            srcStage  = vk::PipelineStageFlagBits::eTransfer;
            srcAccess = vk::AccessFlagBits::eTransferRead;
        }
        else if (layout == vk::ImageLayout::eTransferDstOptimal){
            srcStage  = vk::PipelineStageFlagBits::eTransfer;
            srcAccess = vk::AccessFlagBits::eTransferWrite;
        }
        else if (layout == vk::ImageLayout::eShaderReadOnlyOptimal){
            srcStage  = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;
            srcAccess = vk::AccessFlagBits::eShaderRead;
        }
        else if (layout == vk::ImageLayout::eColorAttachmentOptimal){
            srcStage  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            srcAccess = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        }
        else if (layout == vk::ImageLayout::eDepthStencilAttachmentOptimal){
            srcStage  = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
            srcAccess = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        }
        else if(layout == vk::ImageLayout::eDepthStencilReadOnlyOptimal){
            srcStage  = vk::PipelineStageFlagBits::eTopOfPipe;
            srcAccess = vk::AccessFlagBits::eNone;
        }
        else if (layout == vk::ImageLayout::ePresentSrcKHR){
            srcStage  = vk::PipelineStageFlagBits::eBottomOfPipe;
            srcAccess = vk::AccessFlagBits::eNone;
        }

        // if (newLayout == vk::ImageLayout::eTransferSrcOptimal){
        //     dstStage  = vk::PipelineStageFlagBits::eTransfer;
        //     dstAccess = vk::AccessFlagBits::eTransferRead;
        // }
        // else if (newLayout == vk::ImageLayout::eTransferDstOptimal){
        //     dstStage  = vk::PipelineStageFlagBits::eTransfer;
        //     dstAccess = vk::AccessFlagBits::eTransferWrite;
        // }
        // else if (newLayout == vk::ImageLayout::eShaderReadOnlyOptimal){
        //     dstStage  = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;
        //     dstAccess = vk::AccessFlagBits::eShaderRead;
        // }
        // else if (newLayout == vk::ImageLayout::eColorAttachmentOptimal){
        //     dstStage  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        //     dstAccess = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        // }
        // else if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal){
        //     dstStage  = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
        //     dstAccess = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        // }
        // else if (newLayout == vk::ImageLayout::ePresentSrcKHR){
        //     dstStage  = vk::PipelineStageFlagBits::eBottomOfPipe;
        //     dstAccess = vk::AccessFlagBits::eNone;
        // }
        // else if (newLayout == vk::ImageLayout::eGeneral){
        //     dstStage  = vk::PipelineStageFlagBits::eAllCommands;
        //     dstAccess = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
        // }
        // else if (newLayout == vk::ImageLayout::eDepthStencilReadOnlyOptimal){
        //     dstStage  = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
        //     dstAccess = vk::AccessFlagBits::eDepthStencilAttachmentRead;
        // }

        SetLayout(command, newLayout, srcStage, dstStage, srcAccess, dstAccess);
    }
    void Image::SetLayout(vk::CommandBuffer command, vk::ImageLayout newLayout, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask)noexcept{
        vk::ImageMemoryBarrier barrier = {};
        barrier.sType = vk::StructureType::eImageMemoryBarrier;
        barrier.oldLayout = layout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex =  VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.subresourceRange.layerCount = arrayLayer;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;

        command.pipelineBarrier(srcStage, dstStage, {},{}, {}, barrier);

        layout = newLayout;
    }
    vk::CommandBuffer Image::BeginSingleTimeCommands(const Device&device, const Pool&pool){
        auto command = pool.AllocateCommandBuffers(device);
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        command.begin(beginInfo);
        return command;
    }
    vk::ImageLayout Image::Copy(vk::CommandBuffer command, const Buffer& srcBuffer, vk::ImageLayout dstLayout, vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess, uint32_t imageCount){
        const uint32_t imageSize = size.width * size.height * size.depth * pixelSize;
        SetLayout(command, vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite);
        std::vector<vk::BufferImageCopy> bufferCopyRegions(imageCount);
        for (size_t i = 0; i < imageCount; ++i) {
            bufferCopyRegions[i].imageExtent = size;
            bufferCopyRegions[i].bufferOffset = imageSize * i;
            bufferCopyRegions[i].imageSubresource.layerCount = 1;
            bufferCopyRegions[i].imageSubresource.baseArrayLayer = i;
            bufferCopyRegions[i].imageSubresource.aspectMask = aspectMask;
        }
        command.copyBufferToImage(srcBuffer.GetBuffer(), image, vk::ImageLayout::eTransferDstOptimal, bufferCopyRegions);

        SetLayout(command, dstLayout, vk::PipelineStageFlagBits::eTransfer, dstStage, vk::AccessFlagBits::eTransferWrite, dstAccess);
        return dstLayout;
    }
    void Image::EndSingleTimeCommands(vk::CommandBuffer command, vk::Queue queue, const Device&device, const Pool&pool){
        command.end();
        
        vk::SubmitInfo submitInfo = {};
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command;

        queue.submit(1, &submitInfo, VK_NULL_HANDLE);
        queue.waitIdle();

        pool.FreeCommandBuffers(device, command);
    }
    void Image::Copy(vk::CommandBuffer command, Image&src) {
        assert(format == src.format);
        assert(mipLevels == src.mipLevels);
        assert(size.width == src.size.width && size.height== src.size.height && size.height == src.size.height);

        const uint32_t layerCount = arrayLayer;
        src.SetLayout(command, vk::ImageLayout::eTransferSrcOptimal, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferRead);
        SetLayout(command, vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite);

        std::vector<vk::ImageCopy> regions(mipLevels);
        for (uint32_t i = 0; i < mipLevels; ++i) {
            vk::ImageCopy& r = regions[i];

            r.srcSubresource.aspectMask     = src.aspectMask;
            r.srcSubresource.mipLevel       = i;
            r.srcSubresource.baseArrayLayer = 0;
            r.srcSubresource.layerCount     = layerCount;


            r.dstSubresource.aspectMask     = aspectMask;
            r.dstSubresource.mipLevel       = i;
            r.dstSubresource.baseArrayLayer = 0;
            r.dstSubresource.layerCount     = layerCount;

            r.extent.width  = std::max(size.width  >> i, 1u);
            r.extent.height = std::max(size.height >> i, 1u);
            r.extent.depth  = 1u; // 2D array / cube 都是 1
        }
        command.copyImage(src.image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, regions);

        src.SetLayout(command, vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead);

        SetLayout(command, vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead);
    }

    void Image::Destroy(const Device&device){
        // printf("in function %s: image %p, width %d, height %d\n", __FUNCTION__, image, size.width, size.height);
        vk::Device dev = device;
        vma::Allocator allocator = device.GetAllocator();
        allocator.destroyImage(image, allocation);
        // if(image != VK_NULL_HANDLE)device.destroyImage(image);
        // if(memory != VK_NULL_HANDLE)device.freeMemory(memory);
        if(view != VK_NULL_HANDLE)dev.destroyImageView(view);
        view = VK_NULL_HANDLE;
        image = VK_NULL_HANDLE;
        // memory = VK_NULL_HANDLE;
        layout = vk::ImageLayout::eUndefined;
    }
}
// void VulkanGrayImage::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
//     channels = 1;
//     vk::Extent3D size;
//     size.depth = 1;
//     size.width = width;
//     size.height = height;
//     Create(device, size, vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled, vk::Format::eR8Unorm, arrayLayers);
// }

// void VulkanGrayImage::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width, height);
//     CreateView(device);
//     Copy(device, data, pool, graphics);
// }

// void VulkanGrayImage::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width, height, imageCount);
//     CreateView(device, vk::ImageViewType::e2DArray, imageCount);
//     Copy(device, datas, imageCount, pool, graphics);
// }
// void VulkanFont::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
//     vk::Extent3D size;
//     size.depth = 1;
//     size.width = width;
//     size.height = height;
//     vk::Format format = vk::Format::eR8Unorm;
//     if(channels == 4){
//         format = vk::Format::eR8G8B8A8Unorm;
//     }
//     Create(device, size, vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled, format, arrayLayers);
// }
// void VulkanFont::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, uint32_t channels, vk::Queue graphics, VulkanPool pool){
//     this->channels = channels;
//     CreateImage(device, width, height);
//     CreateView(device);
//     Copy(device, data, pool, graphics);
// }

// void VulkanFont::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, uint32_t channels, vk::Queue graphics, VulkanPool pool){
//     this->channels = channels;
//     CreateImage(device, width, height, imageCount);
//     CreateView(device, vk::ImageViewType::e2DArray, imageCount);
//     Copy(device, datas, imageCount, pool, graphics);
// }

// void VulkanTexture::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
//     vk::Extent3D size;
//     size.depth = 1;
//     size.width = width;
//     size.height = height;
//     Create(device, size, vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled, vk::Format::eR8G8B8A8Unorm, arrayLayers);
// }

// void VulkanTexture::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width, height);
//     CreateView(device);
//     Copy(device, data, pool, graphics);
// }

// void VulkanTexture::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width, height, imageCount);
//     CreateView(device, vk::ImageViewType::e2DArray, imageCount);
//     Copy(device, datas, imageCount, pool, graphics);
// }
// void VulkanStorageImage::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers){
//     vk::Extent3D size;
//     size.depth = 1;
//     size.width = width;
//     size.height = height;
//     Create(device, size, vk::ImageUsageFlagBits::eStorage|vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled, vk::Format::eR8G8B8A8Unorm, arrayLayers);
// }
// void VulkanStorageImage::CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width, height);
//     CreateView(device);
//     Copy(device, data, pool, graphics);
// }
// void VulkanStorageImage::CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width, height, imageCount);
//     CreateView(device, vk::ImageViewType::e2DArray, imageCount);
//     Copy(device, datas, imageCount, pool, graphics);
// }
// void VulkanDepthImage::CreateImage(VulkanDevice device, uint32_t width, uint32_t height, vk::Format format){
//     vk::Extent3D size;
//     size.depth = 1;
//     size.width = width;
//     size.height = height;
//     Create(device, size, vk::ImageUsageFlagBits::eDepthStencilAttachment, format);
//     CreateView(device, vk::ImageAspectFlagBits::eDepth|vk::ImageAspectFlagBits::eStencil);
// }

// void VulkanCubeImage::CreateImage(VulkanDevice device, uint32_t width){
//     vk::Extent3D size;
//     size.depth = 1;
//     size.width = width;
//     size.height = width;
//     Create(device, size, vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled, vk::Format::eR8G8B8A8Unorm, 6);
// }
// void VulkanCubeImage::CreateImage(VulkanDevice device, const void *const *datas, uint32_t width, vk::Queue graphics, VulkanPool pool){
//     CreateImage(device, width);
//     CreateView(device, vk::ImageViewType::eCube, 6);
//     Copy(device, datas, 6, pool, graphics);
// }