#include <iostream>
#include "device.hpp"
namespace vulkan{
    void Device::Cleanup(){
        allocator.destroy();
        device.destroy();
        instance.destroy();
    }
    Device::Device(){
        spdlog::set_level(spdlog::level::debug);
        try{
            mLogger = spdlog::basic_logger_mt("device_logger", "logs/device.logs.txt");            
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }
        spdlog::set_default_logger(mLogger);
    }
    Device::~Device(){
    }
    void Device::CreateAllocator(uint32_t apiVersion){
        vma::VulkanFunctions vmaFns;
        vmaFns.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vmaFns.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

        vma::AllocatorCreateInfo ci{};
        ci.physicalDevice = physical;
        ci.device         = device;
        ci.instance       = instance;
        ci.pVulkanFunctions = &vmaFns;
        ci.vulkanApiVersion = apiVersion;

        assert(device && "device is null, call CreateDevice before calling this function");
        allocator = vma::createAllocator(ci);
    }
    void Device::CreateInstance(const std::vector<const char *> &instanceExtensions){
        std::vector<const char *>layers;
        std::vector<const char *>extensions = instanceExtensions;
        if(state.enableValidation){
            layers.push_back("VK_LAYER_KHRONOS_validation");
            // extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);    
        }
        if(state.enableMeshShader){
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        }
        vk::ApplicationInfo appInfo("", VK_MAKE_VERSION(1,0,0), "", VK_MAKE_VERSION(1,0,0), VK_API_VERSION_1_4);
        vk::InstanceCreateInfo info = {};
        info.setEnabledLayerCount(1);
        info.setPApplicationInfo(&appInfo);
        info.setPEnabledLayerNames(layers);
        info.setPEnabledExtensionNames(extensions);
        info.setEnabledExtensionCount(extensions.size());
        instance = vk::createInstance(info);
        // mLogger->info("applicationName:{}, application version:{}, {}, {}, engine name:{}, engine version:{}, {}, {}, api version:{}, {}, {}",
        //     appInfo.pApplicationName, VK_VERSION_MAJOR(appInfo.applicationVersion), VK_VERSION_MINOR(appInfo.applicationVersion), VK_VERSION_PATCH(appInfo.applicationVersion),
        //     appInfo.pEngineName, VK_VERSION_MAJOR(appInfo.engineVersion), VK_VERSION_MINOR(appInfo.engineVersion), VK_VERSION_PATCH(appInfo.engineVersion),
        //     VK_VERSION_MAJOR(appInfo.apiVersion), VK_VERSION_MINOR(appInfo.apiVersion), VK_VERSION_PATCH(appInfo.apiVersion));
    }
    void Device::CreateDevice(vk::SurfaceKHR surface, const std::vector<const char *> &deviceExtensions){
        const float queuePriorities = 1.0f;
        std::set<uint32_t>uniqueQueueFamilies = {GetQueueFamiliesIndex(vk::QueueFlagBits::eGraphics), GetQueueFamiliesIndex(surface)};
        std::vector<vk::DeviceQueueCreateInfo>queueCreateInfo;
        for(auto queueFamily:uniqueQueueFamilies){
            queueCreateInfo.push_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &queuePriorities));
        }
        std::vector<const char *>extensions = deviceExtensions;
        if(surface != VK_NULL_HANDLE)extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        if(state.enableMeshShader){
            extensions.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
            extensions.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
        }
        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.fillModeNonSolid = VK_TRUE;
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.geometryShader = state.enableGeometryShader;
        deviceFeatures.tessellationShader = state.enableTessellationShader;
        deviceFeatures.fragmentStoresAndAtomics = state.enableFragmentStoresAndAtomics;
        vk::PhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures(state.enableMeshShader, state.enableMeshShader);
        if(state.enableDynamicRendering){
            vk::PhysicalDeviceFeatures2 deviceFeatures2;
            vk::PhysicalDeviceVulkan13Features vk13Features;
            vk13Features.dynamicRendering = VK_TRUE;
            deviceFeatures2.features = deviceFeatures;
            deviceFeatures2.pNext = &vk13Features;
            vk13Features.pNext = &meshShaderFeatures;
            device = physical.createDevice(vk::DeviceCreateInfo({}, queueCreateInfo, {}, extensions, nullptr, &deviceFeatures2));    
        }
        else{
            device = physical.createDevice(vk::DeviceCreateInfo({}, queueCreateInfo, {}, extensions, &deviceFeatures, state.enableMeshShader?&meshShaderFeatures:nullptr));
        }
    }
    bool Device::EnableMeshShader(){
        assert(physical && "physical device is null, call SelectPhysicalDevice before calling this function");
        state.enableMeshShader = IsSupportedExtension(VK_EXT_MESH_SHADER_EXTENSION_NAME);
        if(mLogger)mLogger->info(state.enableMeshShader?"enable mesh shaders":"failed to enable mesh shaders");
        return state.enableMeshShader;
    }
    bool Device::EnableDynamicRendering(){
        assert(physical && "physical device is null, call SelectPhysicalDevice before calling this function");
        state.enableDynamicRendering = IsSupportedExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        if(mLogger)mLogger->info(state.enableDynamicRendering?"enable dynamic rendering":"failed to enable dynamic rendering");
        return state.enableDynamicRendering;
    }
    bool Device::EnableFragmentStoresAndAtomics(){
        assert(physical && "physical device is null, call SelectPhysicalDevice before calling this function");
        vk::PhysicalDeviceFeatures features = physical.getFeatures();
        state.enableFragmentStoresAndAtomics = features.fragmentStoresAndAtomics;
        if(mLogger)mLogger->info(state.enableDynamicRendering?"enable fragment stores and atomics":"failed to enable fragment stores and atomics");
        return state.enableFragmentStoresAndAtomics;
    }
    bool Device::IsSupportedExtension(const char *extensionName) const noexcept{
        auto extensions = physical.enumerateDeviceExtensionProperties();
        for (const auto& ext : extensions) {
            if (!strcmp(ext.extensionName, extensionName)) {
                return true;
            }
        }
        return false;
    }
    void Device::SelectPhysicalDevice(bool (*SelectPhysicalDevice)(vk::PhysicalDevice)){
        std::vector<vk::PhysicalDevice>physicalDevices = instance.enumeratePhysicalDevices();
        if(physicalDevices.empty()){
            if(mLogger)mLogger->error("No suitable graphics card!");
            shutdownlogger();
            assert(0);
            return;
        }
        for (auto& device : physicalDevices) {
            if(SelectPhysicalDevice(device)){
                physical = device;
                break;
            }
        }
        if(!physical){
            physical = physicalDevices[0];
        }
        physicalDeviceProperties = physical.getProperties();
        // memoryProperties = physical.getMemoryProperties();
        queueFamiliesProperties = physical.getQueueFamilyProperties();
        const char *deviceType = "OTHER";
        if(physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eCpu){
            deviceType = "CPU";
        }
        else if(physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu){
            deviceType = "DISCRETE GPU";
        }
        else if(physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu){
            deviceType = "INTEGRATED GPU";
        }
        else if(physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eVirtualGpu){
            deviceType = "VIRTUAL GPU";
        }
        if(mLogger)mLogger->info("device type:{}, device name:{}, api version:{}.{}.{}, driver version:{}, {}, {}",
            deviceType, physicalDeviceProperties.deviceName.data(), VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion), VK_VERSION_MINOR(physicalDeviceProperties.apiVersion), VK_VERSION_PATCH(physicalDeviceProperties.apiVersion),
            VK_VERSION_MAJOR(physicalDeviceProperties.driverVersion), VK_VERSION_MINOR(physicalDeviceProperties.driverVersion), VK_VERSION_PATCH(physicalDeviceProperties.driverVersion));
    }
    uint32_t Device::GetQueueFamiliesIndex(vk::QueueFlagBits queue)const noexcept{
        int32_t queueFamilyIndex = -1;
        for (uint32_t i = 0; i < queueFamiliesProperties.size(); ++i) {
            if (queueFamiliesProperties[i].queueCount > 0 && queueFamiliesProperties[i].queueFlags & queue) {
                queueFamilyIndex = i;
                break;
            }
        }
        return queueFamilyIndex;
    }
    // uint32_t VulkanDevice::findMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags properties){
    //     for (size_t i = 0; i < memoryProperties.memoryTypeCount; i++){
    // 		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
    // 			return i;
    // 		}
    // 	}
    // 	return -1;
    // }
    uint32_t Device::GetVideoQueueFamiliesIndex()const noexcept{
        auto queueFamilies = physical.getQueueFamilyProperties2();
        std::vector<vk::QueueFamilyVideoPropertiesKHR> videoProps(queueFamilies.size());

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            queueFamilies[i].sType = vk::StructureType::eQueueFamilyProperties2;
            queueFamilies[i].pNext = &videoProps[i];

            videoProps[i].sType = vk::StructureType::eQueueFamilyVideoPropertiesKHR;
            videoProps[i].pNext = nullptr;
        }

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            if (videoProps[i].videoCodecOperations & vk::VideoCodecOperationFlagBitsKHR::eDecodeH264) {
                return i;
            }
        }
        return VK_QUEUE_FAMILY_IGNORED;
    }
    uint32_t Device::GetQueueFamiliesIndex(vk::SurfaceKHR surface)const noexcept{
        int32_t queueFamilyIndex = -1;
        VkBool32 presentSupport = false;
        if (surface != VK_NULL_HANDLE){
            //该队列族有呈现能力即可
            for (uint32_t i = 0; i < queueFamiliesProperties.size(); ++i) {
                presentSupport = physical.getSurfaceSupportKHR(i, surface);
                if (presentSupport) {
                    queueFamilyIndex = i;
                    break;
                }
            }
        }
        return queueFamilyIndex;
    }
}