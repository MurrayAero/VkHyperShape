#include "VulkanRenderer.h"
struct SwapChainSupportDetails{
    uint32_t formatCount;
    uint32_t presentModeCount;
    VkSurfaceFormatKHR *formats;
    VkPresentModeKHR *presentModes;
    VkSurfaceCapabilitiesKHR capabilities;
};
void querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, struct SwapChainSupportDetails *details) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details->capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details->formatCount, VK_NULL_HANDLE);
    details->formats = malloc(sizeof(VkFormat) * details->formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &details->formatCount, details->formats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details->presentModeCount, VK_NULL_HANDLE);
    details->presentModes = malloc(sizeof(VkFormat) * details->presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &details->presentModeCount, details->presentModes);
    return details;
}
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t count) {
    const VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkFormat formats[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_UNDEFINED };
    for (int32_t i = 0; i < sizeof(formats) / sizeof(VkFormat); ++i){
        for (int32_t j = 0; j < count; ++j) {
            const VkBool32 formatMatches = (formats[i] == VK_FORMAT_UNDEFINED) || (formats[i] == availableFormats[j].format);
                
            if (formatMatches && (availableFormats[j].colorSpace == colorSpace)) {
                return availableFormats[j];
            }
        }
    }
    return availableFormats[0];
}

// void VulkanWindow::Cleanup(VulkanDevice&device){
//     CleanupSwapchain(device.device);
//     swapchain.Cleanup(device.device);
//     vkDestroySurfaceKHR(device.instance, surface, nullptr);
// }
// void VulkanWindow::CleanupSwapchain(VkDevice device){
//     for (size_t i = 0; i < framebuffers.size(); ++i){
//         vkDestroyFramebuffer(device, framebuffers[i], nullptr);
//     }
//     depthImage.Destroy(device);

//     vkDestroyRenderPass(device, renderPass, nullptr);
// }
// void VulkanWindow::CreateFrameBuffer(VulkanDevice device, bool createDepthImage){
//     uint32_t count;
//     vkGetSwapchainImagesKHR(device.device, swapchain.swapchain, &count, nullptr);
//     std::vector<VkImage>swapchainImages(count);
//     vkGetSwapchainImagesKHR(device.device, swapchain.swapchain, &count, swapchainImages.data());
//     swapchain.images.resize(swapchainImages.size());
//     framebuffers.resize(swapchainImages.size());
//     std::vector<VkImageView>frameBufferAttachments(1);
//     if(createDepthImage){
//         VkExtent3D extent;
//         extent.depth = 1;
//         extent.width = swapchain.extent.width;
//         extent.height = swapchain.extent.height;
//         CreateDepthImage(device, extent, depthImage);
//         frameBufferAttachments.push_back(depthImage.view);
//     }
//     VkFramebufferCreateInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//     info.layers = 1;
//     info.renderPass = renderPass;
//     info.width = swapchain.extent.width;
//     info.height = swapchain.extent.height;
//     info.pAttachments = frameBufferAttachments.data();
//     info.attachmentCount = frameBufferAttachments.size();
//     for (size_t i = 0; i < framebuffers.size(); ++i){
//         swapchain.images[i].image = swapchainImages[i];
//         swapchain.images[i].format = swapchain.surfaceFormat.format;
//         swapchain.images[i].CreateImageView(device.device);
//         frameBufferAttachments[0] = swapchain.images[i].view;
//         vkCreateFramebuffer(device.device, &info, nullptr, &framebuffers[i]);
//     }
// }
// VkResult VulkanWindow::CreateRenderPass(VkDevice device, bool useDepthImage){
//     VkSubpassDependency dependency = {};
//     // dependency.dstSubpass = 0;
//     // dependency.srcAccessMask = 0;
//     dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//     dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

// 	VkAttachmentReference colorAttachmentRef;
//     VkAttachmentReference depthAttachmentRef;
//     colorAttachmentRef.attachment = 0;//指定要引用的附着在附着描述结构体数组中的索引, 头一个,所以是0
//     colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
// 	VkSubpassDescription subPass = {};
//     subPass.colorAttachmentCount = 1;
//     subPass.pColorAttachments = &colorAttachmentRef;
//     subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;//未来可能会支持计算子流程, 所以需要指定这是一个图形子流程
//     // subPass.pInputAttachments = VK_NULL_HANDLE;//被着色器读取的附着
//     // subPass.pResolveAttachments =  VK_NULL_HANDLE;//用于多重采样的颜色附着
//     // subPass.pPreserveAttachments = VK_NULL_HANDLE;//保留的附着, 用于在子流程之间传递数据
//     std::vector<VkAttachmentDescription>attachmentDescription(1);
//     attachmentDescription[0].format = swapchain.surfaceFormat.format;
//     attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
//     attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//     attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//     attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//     attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//     attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//     attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//     if(useDepthImage){
//         depthAttachmentRef.attachment = 1;//指定要引用的附着在附着描述结构体数组中的索引, 下一个, 所以是1
//         depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//         subPass.pDepthStencilAttachment = &depthAttachmentRef;

//         VkAttachmentDescription depthAttachmentDescription = {};
//         depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
//         depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//         depthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
//         depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//         depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//         depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//         depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//         depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//         attachmentDescription.push_back(depthAttachmentDescription);
//     }
//     VkRenderPassCreateInfo info = {};
//     info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//     info.dependencyCount = 1;
//     info.pDependencies = &dependency;
//     info.subpassCount = 1;
//     info.pSubpasses = &subPass;
//     info.pAttachments = attachmentDescription.data();
//     info.attachmentCount = attachmentDescription.size();
//     return vkCreateRenderPass(device, &info, nullptr, &renderPass);
// }
// void VulkanWindow::CreateDepthImage(VulkanDevice device, const VkExtent3D &swapchainExtent, VulkanImage &image){
//     image.CreateImage(device.device, swapchainExtent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_FORMAT_D32_SFLOAT_S8_UINT);
//     image.AllocateAndBindMemory(device, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
//     image.CreateImageView(device.device, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT);
// }
void vulkanCleanupSwapchain(const VulkanDevice *device, VulkanSwapchain *swapchain){
    vkDestroySwapchainKHR(device->device, swapchain->swapchain, VK_NULL_HANDLE);
    for (size_t i = 0; i < swapchain->imageCount; ++i){
        swapchain->images[i].image = VK_NULL_HANDLE;//因为image不是自己创建的所以不能销毁
        vulkanDestroyImage(device, &swapchain->images[i]);
    }
    free(swapchain->images);
}

VulkanSwapchain vulkanCreateSwapchain(const VulkanDevice *device, VkSurfaceKHR surface){
    VulkanSwapchain swapchain;
    struct SwapChainSupportDetails swapChainSupport;
    querySwapChainSupport(device->physical, surface, &swapChainSupport);

    swapchain.surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatCount);

    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    // VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    swapchain.size = swapChainSupport.capabilities.currentExtent;

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    //maxImageCount为0表示没有图片限制
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    // imageCount = std::clamp(imageCount, 2u, 3u);
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;

    info.minImageCount = imageCount;
    info.imageFormat = swapchain.surfaceFormat.format;
    info.imageColorSpace = swapchain.surfaceFormat.colorSpace;
    info.imageExtent = swapchain.size;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { vulkanGetQueueIndex(device, VK_QUEUE_GRAPHICS_BIT), vulkanGetPresentQueueIndex(device, surface) };
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = queueFamilyIndices;
        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    } else {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    info.clipped = VK_TRUE;//设置为VK_TRUE表示我们不关心被窗口系统中的其它窗口遮挡的像素的颜色,这允许vulkan采取一定的优化措施,但如果我们回读窗口的像素值就可能出现问题。
    info.presentMode = presentMode;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.preTransform = swapChainSupport.capabilities.currentTransform;

    info.oldSwapchain = VK_NULL_HANDLE;

    vkCreateSwapchainKHR(device->device, &info, VK_NULL_HANDLE, swapchain.swapchain);

    vkGetSwapchainImagesKHR(device->device, swapchain.swapchain, &swapchain.imageCount, VK_NULL_HANDLE);
    VkImage *swapchainImages = malloc(sizeof(VkImage) * swapchain.imageCount);
    vkGetSwapchainImagesKHR(device->device, swapchain.swapchain, &swapchain.imageCount, swapchainImages);

    swapchain.images = malloc(sizeof(VulkanImage) * swapchain.imageCount);
    for (size_t i = 0; i < swapchain.imageCount; ++i){
        swapchain.images[i].image = swapchainImages[i];
        swapchain.images[i].format = swapchain.surfaceFormat.format;
        vulkanCreateImageView(device->device, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, &swapchain.images[i]);
    }
    free(swapchainImages);
    return swapchain;
}