// #ifndef PICKING_TEXTURE_INCLUDE_H
// #define PICKING_TEXTURE_INCLUDE_H
// #include <string.h>

// #include "vulkanFrame.h"

// // #include "glm/glm.hpp"
// // #include "glm/gtc/matrix_transform.hpp"
// #define DEPTH_FORMAT VK_FORMAT_D32_SFLOAT_S8_UINT
// #define COLOR_FORMAT VK_FORMAT_R32G32B32A32_SFLOAT
// #define PICKING_TEXTURE_CLEAR_SCREEN_VAL .1f
// struct PixelInfo{
//     // glm::vec4 outColor;
// };
// struct PickingTexture{
//     struct {
//         VkPipeline offscreen;
//     } pipelines;
//     struct{
//         VkExtent2D size;
//     }Rect;
//     struct{
//         uint32_t width, height;
//         VkSemaphore semaphore;
//         VkRenderPass renderPass;
//         VulkanImage color, depth;
//         VkFramebuffer frameBuffer;
//     }offscreenPass;
//     VkCommandBuffer mCommand;
//     VulkanImage mCopyColorImage;
//     VkSubresourceLayout subResourceLayout;
//     void DestroyOffscreen(VkDevice device);
//     VkResult CreateOffscreenRenderpass(VkDevice device);
//     VkResult CreateOffscreen(Device device, uint32_t windowWidth, uint32_t windowHeight);
// public:
//     PickingTexture();
//     ~PickingTexture();
//     void Cleanup(VkDevice device);
//     void Setup(Device device, VulkanPool pool, uint32_t scissorWidth, uint32_t scissorHeight);

//     void RecordCommand(void(*Draw)(VkCommandBuffer, const VkPipeline *, uint32_t));
//     void DrawToOffscreen(VkCommandBuffer command, void(*Draw)(VkCommandBuffer, const VkPipeline *, uint32_t));
//     // void DrawOffcreenTexture(VkCommandBuffer command, VkRenderPass renderPass, VkFramebuffer frameBuffer, uint32_t windowWidth, uint32_t windowsHeight);

//     void DestroyGraphicsPipeline(VkDevice device);
//     void CreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout, const VkPipelineCache pipelineCache, VkVertexInputBindingDescription bindingDescriptions, const VkVertexInputAttributeDescription *pAttributeDescriptions, uint32_t count);

//     void DestroyOffscreenFramebuffer(VkDevice device);

//     PixelInfo ReadPixel(VkDevice device, double xpos, double ypos);
//     PixelInfo ReadPixel(VkDevice device, double xpos, double ypos, VkQueue graphics, VulkanPool pool);

//     VkResult Submit(VkQueue graphics, VkSemaphore wait, VkSemaphore &signal);
//     //当画面不一样后，就需要更新图片。包括但不限于摄像机被改变的时候
//     void UpdateImageData(VkDevice device, VkQueue graphics, VulkanPool pool);
// };
// #endif 
// #endif