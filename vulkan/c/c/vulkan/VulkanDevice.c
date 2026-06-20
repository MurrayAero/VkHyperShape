#include "VulkanDevice.h"
void vulkanDeviceCleanup(VulkanDevice* device){
    vmaDestroyAllocator(device->allocator);
    vkDestroyDevice(device->device, VK_NULL_HANDLE);
    vkDestroyInstance(device->instance, VK_NULL_HANDLE);
    free(device->queueFamiliesProperties);
    device->queueFamiliesProperties = NULL;
}
uint32_t vulkanGetQueueIndex(const VulkanDevice *device, VkQueueFlags queue){
    int32_t queueFamilyIndex = -1;
	for (uint32_t i = 0; i < device->queuefamilyCount; ++i) {
        if (device->queueFamiliesProperties[i].queueCount > 0 && device->queueFamiliesProperties[i].queueFlags & queue) {
            queueFamilyIndex = i;
            break;
        }
    }
    return queueFamilyIndex;
}
uint32_t vulkanGetPresentQueueIndex(const VulkanDevice *device, VkSurfaceKHR surface){
    int32_t queueFamilyIndex = -1;
    VkBool32 presentSupport = VK_FALSE;
    if (surface != VK_NULL_HANDLE){
        for (uint32_t i = 0; i < device->queuefamilyCount; ++i) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device->physical, i, surface, &presentSupport);
            if (presentSupport) {
                queueFamilyIndex = i;
                break;
            }
        }
    }
    return queueFamilyIndex;
}
VkResult vulkanSelectPhysicalDevices(VulkanDevice *device, VkBool32 (*SelectPhysicalDevices)(VkPhysicalDevice)){
    uint32_t count;
	vkEnumeratePhysicalDevices(device->instance, &count, VK_NULL_HANDLE);
    if(count == 0){
		return VK_ERROR_INCOMPATIBLE_DRIVER;
    }
    VkPhysicalDevice *physicalDevices = malloc(sizeof(VkPhysicalDevice) * count);
    vkEnumeratePhysicalDevices(device->instance, &count, physicalDevices);
    if(SelectPhysicalDevices){
        for (size_t i = 0; i < count; i++){
            if(SelectPhysicalDevices(physicalDevices[i])){
                device->physical = physicalDevices[i];
                break;
            }
        }
    }
    if(device->physical == VK_NULL_HANDLE){
        device->physical = physicalDevices[0];
    }
    vkGetPhysicalDeviceProperties(device->physical, &device->physicalDeviceProperties);
    // vkGetPhysicalDeviceMemoryProperties(device->physical, &memoryProperties);
    if(device->queueFamiliesProperties == NULL){
        vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &device->queuefamilyCount, VK_NULL_HANDLE);
        device->queueFamiliesProperties = malloc(sizeof(VkQueueFamilyProperties) * device->queuefamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &device->queuefamilyCount, device->queueFamiliesProperties);
    }
    free(physicalDevices);
    return VK_SUCCESS;
}
VkResult vulkanCreateInstance(VulkanDevice *device, const char **extensions, uint32_t count){
    const char *layers;
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_4;
    if(device->state.enableValidation){
        layers = "VK_LAYER_KHRONOS_validation";
    }
    // if(device->state.enableMeshShader){
    //     extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    // }
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.ppEnabledLayerNames = &layers;
    instanceInfo.enabledLayerCount = device->state.enableValidation;
    instanceInfo.enabledExtensionCount = count;
    instanceInfo.ppEnabledExtensionNames = extensions;
    return vkCreateInstance(&instanceInfo, VK_NULL_HANDLE, &device->instance);
}
VkResult vulkanCreateDevice(VulkanDevice *device, VkSurfaceKHR surface, const char **extensions, uint32_t count){
	const float queuePriorities = 1.0f;
    uint32_t graphicsQueueindex = vulkanGetQueueIndex(device, VK_QUEUE_GRAPHICS_BIT), presentQueueindex = vulkanGetPresentQueueIndex(device, surface);
    uint32_t queueCount = graphicsQueueindex == presentQueueindex ? 1 : 2;
    uint32_t *queueFamilies = (uint32_t *)malloc(sizeof(uint32_t) * queueCount);
    VkDeviceQueueCreateInfo *queueCreateInfos = (VkDeviceQueueCreateInfo *)malloc(sizeof(VkDeviceQueueCreateInfo) * queueCount);
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriorities;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    for(uint32_t i = 0; i < queueCount; ++i){
        queueInfo.queueFamilyIndex = queueFamilies[i];
        if(queueInfo.queueFamilyIndex < count)queueCreateInfos[i] = queueInfo;
    }
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures = {};
	vkGetPhysicalDeviceFeatures(device->physical, &features);
	features.samplerAnisotropy = VK_TRUE;//保证该值一定为true
    uint32_t extensionsCount = count;
	const char **extension;
    if(surface != VK_NULL_HANDLE){
        ++extensionsCount;
        extension = malloc(sizeof(const char*) * extensionsCount);
        extension[count] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    }
    VkDeviceCreateInfo deviceCreateInfo = {};
    if(device->state.enableMeshShader){
        meshShaderFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
        meshShaderFeatures.meshShader = VK_TRUE;
        meshShaderFeatures.taskShader = VK_TRUE;
        deviceCreateInfo.pNext = &meshShaderFeatures;
        extensionsCount += 2;
        free(extension);
        extension = malloc(sizeof(const char*) * extensionsCount);
        extension[count] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        extension[count + 1] = VK_EXT_MESH_SHADER_EXTENSION_NAME;
        extension[count + 2] = VK_KHR_SPIRV_1_4_EXTENSION_NAME;
    }
    for (size_t i = 0; i < extensionsCount; i++){
        extension[i] = extensions[i];
    }
    
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
    deviceCreateInfo.pEnabledFeatures = &features;
    deviceCreateInfo.enabledExtensionCount = extensionsCount;
    deviceCreateInfo.ppEnabledExtensionNames = extension;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    deviceCreateInfo.queueCreateInfoCount = queueCount;
    VkResult resutl = vkCreateDevice(device->physical, &deviceCreateInfo, VK_NULL_HANDLE, &device->device);
    free(extension);
    free(queueFamilies);
    free(queueCreateInfos);
    return resutl;
}
// VkResult VulkanDevice::AllocateMemory(VkDeviceSize size, uint32_t typeFilter, VkMemoryPropertyFlags properties, VkDeviceMemory &memory){
//     VkMemoryAllocateInfo allocateInfo;
//     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//     allocateInfo.pNext = nullptr;
//     allocateInfo.allocationSize = size;
//     allocateInfo.memoryTypeIndex = findMemoryTypeIndex(typeFilter, properties);
//     if (allocateInfo.memoryTypeIndex == -1u) {
//         throw std::runtime_error("Failed to find suitable memory type!");
//     }
// 	return vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
// }

// void VulkanDevice::CheckDeviceExtension(const std::vector<const char *> &extensions){
//     uint32_t extensionCount;
//     vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
//     std::vector<VkExtensionProperties>deviceExtensions(extensionCount);
//     vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, deviceExtensions.data());
//     for (auto ext:extensions){
//         bool bSupport = false;
//         for (auto supportExtension:deviceExtensions){
//             if(!strcmp(ext, supportExtension.extensionName)){
//                 bSupport = true;
//                 break;
//             }
//         }
//         if(!bSupport){
//             printf("device extension not supported:%s\n", ext);
//         }
//     }
// }
// void VulkanDevice::CheckInstanceExtension(const std::vector<const char *> &extensions){
//     uint32_t extensionCount;
//     vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
//     std::vector<VkExtensionProperties>instanceExtensions(extensionCount);
//     vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());
//     for (auto ext:extensions){
//         bool bSupport = false;
//         for (auto supportExtension:instanceExtensions){
//             if(!strcmp(ext, supportExtension.extensionName)){
//                 bSupport = true;
//                 break;
//             }
//         }
//         if(!bSupport){
//             printf("instance extension not supported:%s\n", ext);
//         }
//     }
// }
// uint32_t VulkanDevice::GetVideoQueueFamiliesIndex() {
//     uint32_t queueFamilyCount = 0;
//     vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, nullptr);

//     std::vector<VkQueueFamilyProperties2> queueFamilies(queueFamilyCount);
//     std::vector<VkQueueFamilyVideoPropertiesKHR> videoProps(queueFamilyCount);

//     for (uint32_t i = 0; i < queueFamilyCount; ++i) {
//         queueFamilies[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
//         queueFamilies[i].pNext = &videoProps[i];

//         videoProps[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
//         videoProps[i].pNext = nullptr;
//     }

//     vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, queueFamilies.data());

//     for (uint32_t i = 0; i < queueFamilyCount; ++i) {
//         if (videoProps[i].videoCodecOperations & VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR) {
//             return i;
//         }
//     }
//     return VK_QUEUE_FAMILY_IGNORED;
// }
// uint32_t VulkanDevice::findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties){
//     for (size_t i = 0; i < memoryProperties.memoryTypeCount; i++){
// 		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
// 			return i;
// 		}
// 	}
// 	return -1;
// }