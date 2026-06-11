#include "framework.hpp"
const char *cvmx_chip_type_to_string(vk::Result type){
	switch (type) {
	case vk::Result::eSuccess:return "VK_SUCCESS";
	case vk::Result::eNotReady:return "VK_NOT_READY";
	case vk::Result::eTimeout:return "VK_TIMEOUT";
	case vk::Result::eEventSet:return "VK_EVENT_SET";
	case vk::Result::eEventReset:return "VK_EVENT_RESET";
	case vk::Result::eIncomplete:return "VK_INCOMPLETE";
	case vk::Result::eErrorOutOfHostMemory:return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case vk::Result::eErrorOutOfDeviceMemory:return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case vk::Result::eErrorInitializationFailed:return "VK_ERROR_INITIALIZATION_FAILED";
	case vk::Result::eErrorDeviceLost:return "VK_ERROR_DEVICE_LOST";
	case vk::Result::eErrorMemoryMapFailed:return "VK_ERROR_MEMORY_MAP_FAILED";
	case vk::Result::eErrorLayerNotPresent:return "VK_ERROR_LAYER_NOT_PRESENT";
	case vk::Result::eErrorExtensionNotPresent:return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case vk::Result::eErrorFeatureNotPresent:return "VK_ERROR_FEATURE_NOT_PRESENT";
	case vk::Result::eErrorIncompatibleDriver:return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case vk::Result::eErrorTooManyObjects:return "VK_ERROR_TOO_MANY_OBJECTS";
	case vk::Result::eErrorFormatNotSupported:return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case vk::Result::eErrorFragmentedPool:return "VK_ERROR_FRAGMENTED_POOL";
	case vk::Result::eErrorOutOfPoolMemory:return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case vk::Result::eErrorInvalidExternalHandle:return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case vk::Result::eErrorFragmentation:return "VK_ERROR_FRAGMENTATION";
	case vk::Result::eErrorInvalidOpaqueCaptureAddress:return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
	case vk::Result::eErrorSurfaceLostKHR:return "VK_ERROR_SURFACE_LOST_KHR";
	case vk::Result::eErrorNativeWindowInUseKHR:return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case vk::Result::eSuboptimalKHR:return "VK_SUBOPTIMAL_KHR";
	case vk::Result::eErrorOutOfDateKHR:return "VK_ERROR_OUT_OF_DATE_KHR";
	case vk::Result::eErrorIncompatibleDisplayKHR:return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case vk::Result::eErrorValidationFailedEXT:return "VK_ERROR_VALIDATION_FAILED_EXT";
	case vk::Result::eErrorInvalidShaderNV:return "VK_ERROR_INVALID_SHADER_NV";
	case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT:return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case vk::Result::eErrorNotPermittedEXT:return "VK_ERROR_NOT_PERMITTED_EXT";
	default:
		break;
	}
	return "VK_ERROR_UNKNOWN";
}
namespace vulkan{
    namespace framework{
        vk::Sampler CreateTextureSampler(vk::Device device, vk::SamplerAddressMode addressMode, vk::Filter filter){
            vk::SamplerCreateInfo createInfo = {};
            createInfo.sType = vk::StructureType::eSamplerCreateInfo;
            createInfo.magFilter = filter;
            createInfo.minFilter = filter;
            createInfo.addressModeU = addressMode;
            createInfo.addressModeV = addressMode;
            createInfo.addressModeW = addressMode;
            createInfo.anisotropyEnable = VK_TRUE;
            createInfo.maxAnisotropy = 16;
            createInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
            // createInfo.unnormalizedCoordinates = VK_FALSE;
            // createInfo.compareEnable = VK_FALSE;
            createInfo.compareOp = vk::CompareOp::eAlways;
            createInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
            return device.createSampler(createInfo);
        }
        void BeginCommands(vk::CommandBuffer command, vk::CommandBufferUsageFlags flags, vk::CommandBufferInheritanceInfo*inheritanceInfo){
            vk::CommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
            beginInfo.flags = flags;
            beginInfo.pInheritanceInfo = inheritanceInfo;

            command.begin(beginInfo);
        }
        vk::CommandBuffer BeginSingleTimeCommands(vk::Device device, const Pool&pool){
            vk::CommandBuffer command = pool.AllocateCommandBuffers(device);
            BeginCommands(command, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
            return command;
        }
        void EndSingleTimeCommands(vk::CommandBuffer command, vk::Queue queue, vk::Device device, const Pool&pool){
            command.end();
            
            vk::SubmitInfo submitInfo = {};
            submitInfo.sType = vk::StructureType::eSubmitInfo;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &command;

            queue.submit(submitInfo);
            queue.waitIdle();

            pool.FreeCommandBuffers(device, command);
        }
        void BeginRenderPass(vk::CommandBuffer command, vk::Framebuffer frame, vk::RenderPass renderPass, uint32_t windowWidth, uint32_t windowHeight){
            std::vector<vk::ClearValue> clearValues(3);
            clearValues[0].color = { .1f , .1f , .1f , 1.0f };
            clearValues[1].depthStencil =  vk::ClearDepthStencilValue(1.0f, 0);
            vk::RenderPassBeginInfo info = {};
            info.sType = vk::StructureType::eRenderPassBeginInfo;
            info.framebuffer = frame;
            info.renderPass = renderPass;
            info.pClearValues = clearValues.data();
            info.clearValueCount = clearValues.size();
            info.renderArea = { 0, 0, windowWidth, windowHeight };
            command.beginRenderPass(info,vk::SubpassContents::eInline);
        }
        void BeginRendering(vk::CommandBuffer command, const std::vector<Image*>&color, Image&depth, uint32_t windowWidth, uint32_t windowHeight, vk::RenderingFlags flags){
            for (auto&it:color){
                it->SetLayout(command, vk::ImageLayout::eColorAttachmentOptimal, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eColorAttachmentWrite);
            }
            depth.SetLayout(command, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::AccessFlagBits::eDepthStencilAttachmentWrite);
            std::vector<vk::RenderingAttachmentInfo>colorAttachment(color.size());
            for(uint32_t i = 0; i < colorAttachment.size(); ++i){
                colorAttachment[i].imageView = color[i]->GetView();
                colorAttachment[i].imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
                colorAttachment[i].loadOp = vk::AttachmentLoadOp::eClear;
                colorAttachment[i].storeOp = vk::AttachmentStoreOp::eStore;
                colorAttachment[i].clearValue.color = {0.1f, 0.1f, 0.1f, 1.0f};    
            }
            vk::RenderingAttachmentInfo depthAttachment = {};
            depthAttachment.imageView = depth.GetView();
            depthAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
            depthAttachment.clearValue.depthStencil.depth = 1.0f;

            vk::RenderingInfo renderingInfo = {};
            renderingInfo.flags = flags;
            renderingInfo.layerCount = 1;
            renderingInfo.pDepthAttachment = &depthAttachment;
            renderingInfo.pColorAttachments = colorAttachment.data();
            renderingInfo.colorAttachmentCount = colorAttachment.size();
            renderingInfo.renderArea.setOffset(vk::Offset2D(0,0));
            renderingInfo.renderArea.setExtent(vk::Extent2D(windowWidth, windowHeight));
            command.beginRendering(&renderingInfo);
        }
        void EndRendering(vk::CommandBuffer command, Image&color){
            std::vector<Image*>colorattach(1);
            colorattach[0] = &color;
            EndRendering(command, colorattach);
        }

        void EndRendering(vk::CommandBuffer command, const std::vector<Image*>&color){
            command.endRendering();
            for (auto&it:color){
                it->SetLayout(command, vk::ImageLayout::ePresentSrcKHR, vk::PipelineStageFlagBits::eBottomOfPipe, vk::AccessFlagBits::eMemoryRead);
            }
        }
        void UpdateDescriptorSets(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding> &bindings, const std::vector<vulkan::Buffer> &buffer, const std::vector<vulkan::Image> &image, vk::DescriptorSet &destSet, const vk::Sampler &sampler, bool shaderReadOnlyImageLayout){
            std::array<uint32_t, 2>index = {};//一个uniform一个图片采样器。如果需要其他则个数必须增加
            std::vector<vk::WriteDescriptorSet>writeDescriptorSets;
            std::vector<vk::DescriptorBufferInfo>descriptorBufferInfo(buffer.size());
            std::vector<vk::DescriptorImageInfo>descriptorImageInfo(image.size());
            for (size_t i = 0; i < bindings.size(); ++i){
                vk::WriteDescriptorSet writeDescriptorSet;
                writeDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
                writeDescriptorSet.dstSet = destSet;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.dstBinding = bindings[i].binding;
                writeDescriptorSet.descriptorType = bindings[i].descriptorType;
                switch (bindings[i].descriptorType){
                //unlikely表示该分支被执行的概率较低
                [[unlikely]]case vk::DescriptorType::eStorageImage:
                [[unlikely]]case vk::DescriptorType::eInputAttachment:
                //fallthrough表示上一个case没加brak是有意的
                [[likely]]case vk::DescriptorType::eCombinedImageSampler:
                    if(index[1] < image.size()){
                        descriptorImageInfo[index[1]].sampler = sampler;
                        descriptorImageInfo[index[1]].imageView = image[index[1]].GetView();
                        if(bindings[i].descriptorType == vk::DescriptorType::eStorageImage){
                            descriptorImageInfo[index[1]].imageLayout = vk::ImageLayout::eGeneral;
                        }
                        else{
                            if(shaderReadOnlyImageLayout){
                                descriptorImageInfo[index[1]].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                            }
                            else{
                                descriptorImageInfo[index[1]].imageLayout = image[index[1]].GetLayout();
                            }
                        }
                        writeDescriptorSet.pImageInfo = &descriptorImageInfo[index[1]++];
                        writeDescriptorSets.push_back(writeDescriptorSet);
                    }
                    break;
                [[likely]]case vk::DescriptorType::eUniformBuffer:
                [[unlikely]]case vk::DescriptorType::eStorageBuffer:
                [[likely]]case vk::DescriptorType::eUniformBufferDynamic:
                [[unlikely]]case vk::DescriptorType::eStorageBufferDynamic:
                    if(index[0] < buffer.size()){
                    descriptorBufferInfo[index[0]].offset = 0;
                        descriptorBufferInfo[index[0]].range = buffer[index[0]].GetSize();
                        descriptorBufferInfo[index[0]].buffer = buffer[index[0]].GetBuffer();
                        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo[index[0]++];
                        writeDescriptorSets.push_back(writeDescriptorSet);
                    }
                    break;
                [[unlikely]]default:
                    break;
                }
            }
            device.updateDescriptorSets(writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
        }
    }
}