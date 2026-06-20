#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H
#include <stdint.h>
#include <stdlib.h>
#include <vk_mem_alloc.h>
struct VULKAN_DEVICE{
    struct{
        VkBool32 enableValidation;
        VkBool32 enableMeshShader;
    }state;
    VkDevice device;
    VkInstance instance;
    VmaAllocator allocator;
    VkPhysicalDevice physical;
    uint32_t queuefamilyCount;
    VkQueueFamilyProperties *queueFamiliesProperties;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    // VkPhysicalDeviceMemoryProperties memoryProperties;

//     void CheckDeviceExtension(const std::vector<const char*>&extensions);
//     void CheckInstanceExtension(const std::vector<const char*>&extensions);

//     uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);

//     void SelectPhysicalDevices(VkBool32 (*SelectPhysicalDevices)(VkPhysicalDevice) = nullptr);
//     uint32_t GetVideoQueueFamiliesIndex();

};
typedef struct VULKAN_DEVICE VulkanDevice;

VkResult vulkanCreateInstance(VulkanDevice* device, const char **extensions, uint32_t count);
VkResult vulkanCreateDevice(VulkanDevice* device, VkSurfaceKHR surface, const char **deviceExtensions, uint32_t count);

void vulkanDeviceCleanup(VulkanDevice* device);

uint32_t vulkanGetQueueIndex(const VulkanDevice *device, VkQueueFlags queue);
uint32_t vulkanGetPresentQueueIndex(const VulkanDevice *device, VkSurfaceKHR surface);

VkResult vulkanSelectPhysicalDevices(VulkanDevice *device, VkBool32 (*SelectPhysicalDevices)(VkPhysicalDevice));
#endif