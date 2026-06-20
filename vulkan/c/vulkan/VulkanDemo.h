// #ifndef VULKAN_DEMO_H
// #define VULKAN_DEMO_H

// // #include "Pipeline.hpp"
// #include "vulkanFrame.h"
// #include "vulkanPrimitive.h"
// struct Vertex {
//     glm::vec3 pos;
//     glm::vec3 color;
//     Vertex(){
        
//     }
//     Vertex(const glm::vec3&pos, const glm::vec3&color){
//         this->pos = pos;
//         this->color = color;
//     }
//     static VkVertexInputBindingDescription inputBindingDescription(uint32_t binding) {
//         return VkVertexInputBindingDescription({ binding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
//     }
//     static VkVertexInputAttributeDescription inputAttributeDescription(uint32_t location, VkFormat format, uint32_t offset, uint32_t binding = 0){
//         VkVertexInputAttributeDescription info;
//         info.offset = offset;
//         info.format = format;
//         info.binding = binding;
//         info.location = location;
//         return info;
//     }
//     static std::array<VkVertexInputAttributeDescription, 2> inputAttributeDescriptions(uint32_t binding) {
//         return { inputAttributeDescription(0, VK_FORMAT_R32G32B32_SFLOAT, 0, binding), inputAttributeDescription(1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::color), binding) };
//     }
// };
// class VulkanDemo{
//     // VulkanSphere mSphere;
//     VkDescriptorSet mSet;
//     VulkanPrimitive mCube;
//     VulkanCubeImage mTexture;
//     VkSampler mTextureSampler;
//     void CreateCube(VulkanDevice device, VkQueue graphics, VulkanPool pool);
// public:
//     VulkanDemo(/* args */);
//     ~VulkanDemo();

//     void Cleanup(VkDevice device);
//     void Setup(VulkanDevice device, VkDescriptorSetLayout layout, VkQueue graphics, VulkanPool pool);

//     void Draw(VkCommandBuffer command, VkPipelineLayout layout);

//     void UpdateSet(VkDevice device, VulkanBuffer camera);
// };
// #endif