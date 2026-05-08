#ifndef VULKAN_DEVICE_HPP
#define VULKAN_DEVICE_HPP
#include <set>
#include <vector>
#include <vk_mem_alloc.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
namespace vulkan{
    class Device{
    public:
        Device();
        Device(const Device&) = delete;
        ~Device();
        void Cleanup();
        void CreateAllocator(uint32_t apiVersion = VK_API_VERSION_1_4);
        void CreateInstance(const std::vector<const char*>&instanceExtensions = {});
        void CreateDevice(vk::SurfaceKHR surface, const std::vector<const char*>& deviceExtensions = {});

        bool EnableMeshShader();
        bool EnableDynamicRendering();
        bool EnableFragmentStoresAndAtomics();

        bool IsSupportedExtension(const char* extensionName)const noexcept;

    // uint32_t findMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

        void SelectPhysicalDevice(bool (*SelectPhysicalDevices)(vk::PhysicalDevice) = nullptr);
        
        uint32_t GetVideoQueueFamiliesIndex()const noexcept;
        uint32_t GetQueueFamiliesIndex(vk::SurfaceKHR surface)const noexcept;
        uint32_t GetQueueFamiliesIndex(vk::QueueFlagBits queue)const noexcept;

        inline operator vk::Device() const noexcept{
            return device;
        }
        // inline operator vma::Allocator() const noexcept{
        //     return allocator;
        // }
        inline void EnableValidation()noexcept{
            state.enableValidation = true;
        }
        inline void EnableGeometryShader()noexcept{
            state.enableGeometryShader = true;
        }
        inline void EnableTessellationShader()noexcept{
            state.enableTessellationShader= true;
        }
        //nodiscard表示返回值应该被使用
        [[nodiscard]]inline auto&GetDevice()const noexcept{
            return device;
        }
        inline auto&GetInstance()const noexcept{
            return instance;
        }
        inline auto&GetPhysicalDevice()const noexcept{
            return physical;
        }
        inline auto&GetAllocator()const noexcept{
            return allocator;
        }
        inline auto&GetPhysicalDeviceProperties()const noexcept{
            return physicalDeviceProperties;
        }
        
        inline auto IsEnableMeshShader()const noexcept{
            return state.enableMeshShader;
        }
        inline auto IsEnableDynamicRendering()const noexcept{
            return state.enableDynamicRendering;
        }
        inline void waitIdle()const{
            device.waitIdle();
        }
    private: 
        struct{
            bool enableValidation;
            bool enableMeshShader;
            bool enableGeometryShader;
            bool enableDynamicRendering;
            bool enableTessellationShader;
            bool enableFragmentStoresAndAtomics;
        }state;
        
        vk::Device device = {};
        vk::Instance instance = {};
        vma::Allocator allocator = {};
        vk::PhysicalDevice physical = {};

        std::shared_ptr<spdlog::logger>mLogger;

        vk::PhysicalDeviceProperties physicalDeviceProperties = {};
        // vk::PhysicalDeviceMemoryProperties memoryProperties = {};
        std::vector<vk::QueueFamilyProperties>queueFamiliesProperties;
        inline void shutdownlogger(){
            spdlog::default_logger()->flush();
            spdlog::shutdown();
        }
    };
}
#endif