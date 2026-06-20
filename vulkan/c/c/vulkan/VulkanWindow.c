// #include "VulkanWindow.h"
// struct SwapChainSupportDetails {
//     VkSurfaceCapabilitiesKHR capabilities;
//     std::vector<VkSurfaceFormatKHR> formats;
//     std::vector<VkPresentModeKHR> presentModes;
// };
// SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
//     SwapChainSupportDetails details;

//     vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

//     uint32_t formatCount;
//     vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

//     if (formatCount != 0) {
//         details.formats.resize(formatCount);
//         vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
//     }

//     uint32_t presentModeCount;
//     vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

//     if (presentModeCount != 0) {
//         details.presentModes.resize(presentModeCount);
//         vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
//     }

//     return details;
// }
// VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
//     const std::vector<std::pair<VkFormat, VkColorSpaceKHR>> priorityList = {
//         // 最佳选择：UNORM 格式 + sRGB 非线性颜色空间（广泛兼容）
//         {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
//         {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},

//         // 备用选择：设备支持的其他格式，但避免 SRGB 自动转换
//         {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
//         {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},

//         // 兜底：直接使用第一个可用格式
//         {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
//     };
//     for (const auto& preferred : priorityList) {
//         for (const auto& available : availableFormats) {
//             const bool formatMatches = (preferred.first == VK_FORMAT_UNDEFINED) || (preferred.first == available.format);
                
//             if (formatMatches && (preferred.second == available.colorSpace)) {
//                 return available;
//             }
//         }
//     }
//     for (const auto& preferred : priorityList) {
//         for (const auto& available : availableFormats) {
//             if (preferred.first == available.format) {
//                 return available;
//             }
//         }
//     }
//     return availableFormats[0];
// }
// // VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
// //     if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
// //         return capabilities.currentExtent;
// //     } else {
// //         int width, height;
// //         glfwGetFramebufferSize(window, &width, &height);

// //         VkExtent2D actualExtent = {
// //             static_cast<uint32_t>(width),
// //             static_cast<uint32_t>(height)
// //         };

// //         actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
// //         actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

// //         return actualExtent;
// //     }
// // }

// VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
//     VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
//     for (const auto& availablePresentMode : availablePresentModes) {
//         if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
//             presentMode = availablePresentMode;
//             break;
//         }
//     }
//     if(presentMode  == VK_PRESENT_MODE_FIFO_KHR){
//         for (const auto& availablePresentMode : availablePresentModes) {
//             if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR){
//                 presentMode = availablePresentMode;
//                 break;
//             }
//         }
//     }
//     return presentMode;
// }

// VkResult VulkanSwapchain::CreateSwapchain(VulkanDevice device, VkSurfaceKHR surface){
//     SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device.physicalDevice, surface);

//     surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
//     VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
//     // VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

//     uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
//     //maxImageCount为0表示没有图片限制
//     if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
//         imageCount = swapChainSupport.capabilities.maxImageCount;
//     }
//     // imageCount = std::clamp(imageCount, 2u, 3u);
//     extent = swapChainSupport.capabilities.currentExtent;

//     VkSwapchainCreateInfoKHR createInfo{};
//     createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//     createInfo.surface = surface;

//     createInfo.minImageCount = imageCount;
//     createInfo.imageFormat = surfaceFormat.format;
//     createInfo.imageColorSpace = surfaceFormat.colorSpace;
//     createInfo.imageExtent = swapChainSupport.capabilities.currentExtent;
//     createInfo.imageArrayLayers = 1;
//     createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

//     uint32_t queueFamilyIndices[2] = { device.GetQueueFamiliesIndex(VK_QUEUE_GRAPHICS_BIT), device.GetQueueFamiliesIndex(surface) };
//     if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
//         createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//         createInfo.queueFamilyIndexCount = 2;
//         createInfo.pQueueFamilyIndices = queueFamilyIndices;
//     } else {
//         createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//     }

//     createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
//     createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//     createInfo.presentMode = presentMode;
//     createInfo.clipped = VK_TRUE;//设置为VK_TRUE表示我们不关心被窗口系统中的其它窗口遮挡的像素的颜色,这允许vulkan采取一定的优化措施,但如果我们回读窗口的像素值就可能出现问题。

//     createInfo.oldSwapchain = VK_NULL_HANDLE;

//     return vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swapchain);
// }

// void VulkanSwapchain::Cleanup(VkDevice device){
//     vkDestroySwapchainKHR(device, swapchain, nullptr);
//     for (size_t i = 0; i < images.size(); ++i){
//         images[i].image = VK_NULL_HANDLE;//因为image不是自己创建的所以不能销毁
//         images[i].Destroy(device);
//     }
// }
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