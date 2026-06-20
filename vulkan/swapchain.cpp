#include "swapchain.hpp"
struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR>formats;
    std::vector<vk::PresentModeKHR>presentModes;
};
SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
    SwapChainSupportDetails details;
    details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
    details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
    return details;
}
vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    const std::vector<std::pair<vk::Format, vk::ColorSpaceKHR>> priorityList = {
        // 最佳选择：UNORM 格式 + sRGB 非线性颜色空间（广泛兼容）
        { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
        { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},

        // 备用选择：设备支持的其他格式，但避免 SRGB 自动转换
        { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},

        // 兜底：直接使用第一个可用格式
        { vk::Format::eUndefined, vk::ColorSpaceKHR::eSrgbNonlinear}
    };
    for (const auto& preferred : priorityList) {
        for (const auto& available : availableFormats) {
            const bool formatMatches = (preferred.first == vk::Format::eUndefined) || (preferred.first == available.format);
                
            if (formatMatches && (preferred.second == available.colorSpace)) {
                return available;
            }
        }
    }
    for (const auto& preferred : priorityList) {
        for (const auto& available : availableFormats) {
            if (preferred.first == available.format) {
                return available;
            }
        }
    }

    return availableFormats[0];
}
vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) {
    constexpr std::array preferredModes = {
        vk::PresentModeKHR::eMailbox,// 最佳平衡：垂直同步+低延迟
        vk::PresentModeKHR::eFifo,// 严格垂直同步
        vk::PresentModeKHR::eFifoRelaxed,// 自适应垂直同步
        vk::PresentModeKHR::eImmediate// 无垂直同步
    };
    for (auto preferred : preferredModes) {
        if (std::find(availableModes.begin(), availableModes.end(), preferred) != availableModes.end()) {
            return preferred;
        }
    }
    return availableModes.empty() ? vk::PresentModeKHR::eFifo : availableModes[0];
}
namespace vulkan{
    Swapchain::Swapchain(){
    }

    Swapchain::~Swapchain(){
    }

    void Swapchain::Create(const Device&device, vk::SurfaceKHR surface, uint32_t width, uint32_t height){
        vk::Device dev = device;
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device.GetPhysicalDevice(), surface);

        surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        // VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        if(swapChainSupport.capabilities.currentExtent.width != UINT32_MAX){
            extent = swapChainSupport.capabilities.currentExtent;
        }
        else{
            //有一些桌面环境无法通过getSurfaceCapabilitiesKHR获得窗口大小，例如wayland
            extent.width = std::clamp(width, swapChainSupport.capabilities.minImageExtent.width, swapChainSupport.capabilities.maxImageExtent.width);
            extent.height = std::clamp(height, swapChainSupport.capabilities.minImageExtent.height, swapChainSupport.capabilities.maxImageExtent.height);
        }

        uint32_t imageCount = std::clamp(swapChainSupport.capabilities.minImageCount + 1, 2u, 3u);
        //maxImageCount为0表示没有图片限制
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        // imageCount = std::clamp(imageCount, 2u, 3u);
        vk::SwapchainCreateInfoKHR info = {};
        info.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        info.surface = surface;

        info.minImageCount = imageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        uint32_t queueFamilyIndices[2] = { device.GetQueueFamiliesIndex(vk::QueueFlagBits::eGraphics), device.GetQueueFamiliesIndex(surface) };
        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            info.imageSharingMode = vk::SharingMode::eConcurrent;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            info.imageSharingMode = vk::SharingMode::eExclusive;
        }
        info.clipped = vk::True;//设置为VK_TRUE表示我们不关心被窗口系统中的其它窗口遮挡的像素的颜色,这允许vulkan采取一定的优化措施,但如果我们回读窗口的像素值就可能出现问题。
        info.presentMode = presentMode;
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        info.preTransform = swapChainSupport.capabilities.currentTransform;

        info.oldSwapchain = VK_NULL_HANDLE;

        swapchain = dev.createSwapchainKHR(info);

        std::vector<vk::Image>swapchainImages = dev.getSwapchainImagesKHR(swapchain);
        images.resize(swapchainImages.size());
        for (size_t i = 0; i < images.size(); ++i){
            images[i].SetImage(swapchainImages[i]);
            images[i].SetFormt(surfaceFormat.format);
            images[i].CreateView(device);       
        }    
    }
    void Swapchain::Cleanup(const Device&device){
        vk::Device dev = device;
        dev.destroySwapchainKHR(swapchain);
        for (size_t i = 0; i < images.size(); ++i){
            images[i].SetImage(VK_NULL_HANDLE);//因为image不是自己创建的所以不能销毁
            images[i].Destroy(device);
        }
    }
}