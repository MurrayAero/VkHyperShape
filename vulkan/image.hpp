#ifndef VULKAN_IMAGE_HPP
#define VULKAN_IMAGE_HPP
#include <cstring>
#include "buffer.hpp"
namespace vulkan{
    class Image{
    public:
        //调用该函数前需要先调用各种Set*
        void Create(const Device&device, vk::ImageUsageFlags usage);
        //在大小不一样的情况下才会重新创建图片;其他情况下，想重建图片应该先销毁图片再重建
        void Create(const Device&device, const vk::Extent3D&extent, vk::ImageUsageFlags usage, vk::Format format);
        void Create(const Device&device, const vk::Extent3D&extent, vk::ImageUsageFlags usage, vk::Format format, uint32_t arrayLayer, vk::ImageTiling tiling = vk::ImageTiling::eOptimal, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1);

        void CreateView(vk::Device device, vk::ImageViewType type = vk::ImageViewType::e2D);

        void Copy(vk::CommandBuffer command, Image&src);
        template <typename ImageType>
        void Copy(const Device& device, const ImageType* datas, uint32_t count, vk::Queue queue, const Pool& pool) {
            std::vector<ImageType*>imageDatas(count);
            const uint32_t imageSize = size.width * size.height * size.depth * pixelSize;
            for (size_t i = 0; i < count; i++) {
                imageDatas[i] = new ImageType[imageSize];
                memcpy(imageDatas[i], datas + imageSize * i, imageSize);
            }
            Copy(device, imageDatas, queue, pool);
            for (size_t i = 0; i < count; i++) {
                delete[]imageDatas[i];
            }
        }
        template <typename ImageType>
        void Copy(const Device& device, const std::vector<ImageType>& datas, vk::Queue queue, const Pool& pool) {
            Buffer tempStorageBuffer;
            const uint32_t imageSize = size.width * size.height * size.depth * pixelSize;
            tempStorageBuffer.Create(device, imageSize * datas.size(), vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuToGpu);
            for (size_t i = 0; i < datas.size(); i++) {
                tempStorageBuffer.UpdateData(device, imageSize, datas[i], i * imageSize);
            }
            vk::CommandBuffer command = BeginSingleTimeCommands(device, pool);
            Copy(command, tempStorageBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead, datas.size());
            EndSingleTimeCommands(command, queue, device, pool);
            tempStorageBuffer.Destroy(device);
        }
        
        vk::ImageLayout Copy(vk::CommandBuffer command, const Buffer& srcBuffer, vk::ImageLayout dstLayout, vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess, uint32_t imageCount = 1);

        void Destroy(const Device&device);

        void SetLayout(vk::CommandBuffer command, vk::ImageLayout newLayout, vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess)noexcept;
        void SetLayout(vk::CommandBuffer command, vk::ImageLayout newLayout, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask)noexcept;
        // //通过map更新图片数据
        // void UpdateImage(VulkanDevice device, const void *datas, uint32_t imageCount = 1);
        inline operator vk::Image() const noexcept{
            return image;
        }
        inline bool operator==(vk::Image img)const noexcept{
            return image == img;
        }
        inline bool operator!=(vk::Image img)const noexcept{
            return image != img;
        }
        inline auto&GetFormat()const noexcept{
            return format;
        }
        inline auto&GetImage()const noexcept{
            return image;
        }
        inline auto&GetView()const noexcept{
            return view;
        }
        inline auto&GetSize()const noexcept{
            return size;
        }
        inline uint32_t GetMipLevel()const noexcept{
            return mipLevels;
        }
        inline auto&GetMemory()const noexcept{
            return allocation;
        }
        inline auto&GetLayout()const noexcept{
            return layout;
        }
        inline auto GetAspectMask()const noexcept{
            return aspectMask;
        }
        inline uint32_t GetArrayLayer()const noexcept{
            return arrayLayer;
        }
        inline void *MapMemory(Device device){
            vma::Allocator allocator = device.GetAllocator();
            return allocator.mapMemory(allocation);
        }
        inline void SetAspectMask(vk::ImageAspectFlags aspectMask)noexcept{
            this->aspectMask = aspectMask;
        }
        inline void SetWidth(uint32_t width)noexcept{
            size.width = width;
        }
        inline void SetHeight(uint32_t height)noexcept{
            size.height = height;
        }
        inline void SetDepth(uint32_t depth)noexcept{
            size.depth = depth;
        }
        inline void SetSize(vk::Extent3D size)noexcept{
            this->size = size;
        }
        inline void SetMipLevels(uint32_t level)noexcept{
            mipLevels = level;
        }
        inline void SetFormt(vk::Format format)noexcept{
            this->format = format;
        }
        inline void SetImage(vk::Image image)noexcept{
            this->image = image;
        }
        inline void Settiling(vk::ImageTiling tiling)noexcept{
            this->tiling = tiling;
        }
        inline void SetArrayLayer(uint32_t arrayLayer)noexcept{
            this->arrayLayer = arrayLayer;
        }
        inline void SetLayout(vk::ImageLayout layout)noexcept{
            this->layout = layout;
        }
        inline void SetSamples(vk::SampleCountFlagBits samples)noexcept{
            this->samples = samples;
        }
        inline void SetPixelSize(uint32_t pixelSize)noexcept{
            this->pixelSize = pixelSize;
        }
        inline void SetMemory(const vma::Allocation&allocation){
            this->allocation = allocation;
        }
        inline void UnmapMemory(Device device){
            vma::Allocator allocator = device.GetAllocator();
            allocator.unmapMemory(allocation);
        }
    private:
        vk::Format format;
        uint32_t pixelSize = 4;
        uint32_t mipLevels = 1;
        uint32_t arrayLayer = 1;
        vma::Allocation allocation;
        vk::Extent3D size = { 1, 1, 1 };
        vk::Image image = VK_NULL_HANDLE;
        vk::ImageView view = VK_NULL_HANDLE;
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
        vk::ImageLayout layout = vk::ImageLayout::eUndefined;
        vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1;
        vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
        // vk::DeviceMemory memory = VK_NULL_HANDLE;
        vk::CommandBuffer BeginSingleTimeCommands(const Device&device, const Pool&pool);

        void EndSingleTimeCommands(vk::CommandBuffer command, vk::Queue queue, const Device&device, const Pool&pool);
    };
}
// class VulkanGrayImage:public VulkanImage{
// public:
//     void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
//     void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool);
//     void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool);
// };
// class VulkanFont:public VulkanImage{
// public:
//     void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
//     void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, uint32_t channels, vk::Queue graphics, VulkanPool pool);
//     void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, uint32_t channels, vk::Queue graphics, VulkanPool pool);
// };
// class VulkanTexture:public VulkanImage{
// public:
//     void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
//     void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool);
//     void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool);
// };
// class VulkanStorageImage:public VulkanImage{
// public:
//     void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, uint32_t arrayLayers = 1);
//     void CreateImage(VulkanDevice device, const void *data, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool);
//     void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, uint32_t height, vk::Queue graphics, VulkanPool pool);
// };
// class VulkanDepthImage:public VulkanImage{
// public:
//     void CreateImage(VulkanDevice device, uint32_t width, uint32_t height, vk::Format format = vk::Format::eD32SfloatS8Uint);
// };
// class VulkanCubeImage:public VulkanImage{
// public:
//     void CreateImage(VulkanDevice device, uint32_t width);
//     void CreateImage(VulkanDevice device, const void *const *datas, uint32_t width, vk::Queue graphics, VulkanPool pool);
//     //立方体贴图本身就是图片数组, 那么立方体纹理数组要怎么写?arraylayer = imagecount*6?
//     // void CreateImageArray(VulkanDevice device, const void *const *datas, uint32_t imageCount, uint32_t width, vk::Queue graphics, VulkanPool pool);
//     /*
//         .vert
//         // 立方体贴图使用 ​​从立方体中心出发的观察方向​​ 作为纹理坐标，而非UV坐标
//         out vec3 fragDir = mat3(model) * modelPos;
//         .frag
//         // 声明立方体贴图数组采样器
//         layout(set = 0, binding = 0) uniform samplerCubeArray texCubeArray;

//         // 采样函数
//         void main() {
//             // 1. 创建采样方向向量（传统立方体贴图使用的vec3）
//             vec3 direction = normalize(fragmentDirection);
            
//             // 2. 添加数组索引作为第四分量
//             float cubeIndex = ;//float(gl_InstanceIndex); // 从实例索引获取
//             vec4 cubemapCoord = vec4(direction, cubeIndex);
            
//             // 3. 采样立方体数组
//             vec4 color = texture(texCubeArray, cubemapCoord);
//         }
//     */
// };
#endif