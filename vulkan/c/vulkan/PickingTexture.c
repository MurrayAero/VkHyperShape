// #include "PickingTexture.h"
// void PickingTexture::DestroyOffscreen(VkDevice device){
//     vkDestroyRenderPass(device, offscreenPass.renderPass, nullptr);
// }
// VkResult PickingTexture::CreateOffscreenRenderpass(VkDevice device){
//     std::array<VkAttachmentDescription, 2>attachmentDescription{};
//     attachmentDescription[0].format = COLOR_FORMAT;
//     attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
//     attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//     attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
//     attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//     attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//     attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
//     attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

//     attachmentDescription[1].format = DEPTH_FORMAT;
//     attachmentDescription[1].samples = VK_SAMPLE_COUNT_1_BIT;
//     attachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
//     attachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
//     attachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//     attachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//     attachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
//     attachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

//     VkAttachmentReference attachmentReference = {};
//     attachmentReference.attachment = 0;
//     attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//     VkAttachmentReference depthReference = {};
//     depthReference.attachment = 1;
//     depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

//     VkSubpassDescription subpass = {};
//     subpass.colorAttachmentCount = 1;												// No color attachments
//     subpass.pColorAttachments = &attachmentReference;
//     subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment
//     subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

//     // Use subpass dependencies for layout transitions
//     std::array<VkSubpassDependency, 2> dependencies;
//     dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
//     dependencies[0].dstSubpass = 0;
//     dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//     dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//     dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//     dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

//     dependencies[1].srcSubpass = 0;
//     dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
//     dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//     dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//     dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//     dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//     dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

//     VkRenderPassCreateInfo renderPassCreateInfo = {};
//     renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//     renderPassCreateInfo.subpassCount = 1;
//     renderPassCreateInfo.pSubpasses = &subpass;
//     renderPassCreateInfo.pDependencies = dependencies.data();
//     renderPassCreateInfo.pAttachments = attachmentDescription.data();
//     renderPassCreateInfo.attachmentCount = attachmentDescription.size();
//     renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());

//     return vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &offscreenPass.renderPass);
// }
// VkResult PickingTexture::CreateOffscreen(Device device, uint32_t windowWidth, uint32_t windowHeight){
//     offscreenPass.width = windowWidth;
//     offscreenPass.height = windowHeight;

//     offscreenPass.color.size.depth = 1;
//     offscreenPass.color.size.width = offscreenPass.width;
//     offscreenPass.color.size.height = offscreenPass.height;
//     offscreenPass.color.CreateImage(device.device, offscreenPass.color.size, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, COLOR_FORMAT);// We will sample directly from the color attachment for the shadow mapping
//     offscreenPass.color.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//     offscreenPass.color.CreateImageView(device.device);
//     // For shadow mapping we only need a depth attachment// Depth stencil attachment
//     offscreenPass.depth.size.depth = 1;
//     offscreenPass.depth.size.width = offscreenPass.width;
//     offscreenPass.depth.size.height = offscreenPass.height;
//     offscreenPass.depth.CreateImage(device.device, offscreenPass.depth.size, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, DEPTH_FORMAT);// We will sample directly from the depth attachment for the shadow mapping
//     offscreenPass.depth.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//     offscreenPass.depth.CreateImageView(device.device, VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT);

//     CreateOffscreenRenderpass(device.device);

//     std::array<VkImageView, 2>attachment = { offscreenPass.color.view, offscreenPass.depth.view };
//     VkFramebufferCreateInfo frameBufferInfo = {};
//     frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//     frameBufferInfo.layers = 1;
//     frameBufferInfo.width = offscreenPass.width;
//     frameBufferInfo.height = offscreenPass.height;
//     frameBufferInfo.renderPass = offscreenPass.renderPass;
//     frameBufferInfo.pAttachments = attachment.data();
//     frameBufferInfo.attachmentCount = attachment.size();
//     return vkCreateFramebuffer(device.device, &frameBufferInfo, nullptr, &offscreenPass.frameBuffer);
// }
// PickingTexture::PickingTexture(){
// }

// PickingTexture::~PickingTexture(){
// }

// void PickingTexture::Cleanup(VkDevice device){
//     mCopyColorImage.Destroy(device);
//     vkDestroySemaphore(device, offscreenPass.semaphore, nullptr);
//     DestroyOffscreenFramebuffer(device);
    
//     vkDestroyPipeline(device, pipelines.offscreen, VK_NULL_HANDLE);
// }
// void PickingTexture::Setup(Device device, VulkanPool pool, uint32_t scissorWidth, uint32_t scissorHeight){
//     CreateOffscreen(device, scissorWidth, scissorHeight);

//     mCopyColorImage.size = offscreenPass.color.size;
//     mCopyColorImage.CreateImage(device.device, mCopyColorImage.size, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, COLOR_FORMAT, VK_IMAGE_TILING_LINEAR);
//     // mCopyColorImage.CreateImage(device, VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, COLOR_FORMAT, VK_IMAGE_TILING_LINEAR);
//     mCopyColorImage.AllocateAndBindMemory(device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//     mCopyColorImage.CreateImageView(device.device);

//     VkSemaphoreCreateInfo semaphoreCreateInfo = {};
//     semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//     vkCreateSemaphore(device.device, &semaphoreCreateInfo, nullptr, &offscreenPass.semaphore);

//     pool.AllocateCommandBuffers(device.device, 1, &mCommand);
// }

// void PickingTexture::RecordCommand(void(*Draw)(VkCommandBuffer, const VkPipeline *, uint32_t)){
//     vulkanFrame::BeginCommands(mCommand, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
//     DrawToOffscreen(mCommand, Draw);
//     vkEndCommandBuffer(mCommand);
// }
// void PickingTexture::DrawToOffscreen(VkCommandBuffer command, void (*Draw)(VkCommandBuffer, const VkPipeline *, uint32_t)){
//     vulkanFrame::BeginRenderPass(command, offscreenPass.frameBuffer, offscreenPass.renderPass, offscreenPass.width, offscreenPass.height);

//     Draw(command, &pipelines.offscreen, 1);

//     vkCmdEndRenderPass(command);
// }
// void PickingTexture::DestroyGraphicsPipeline(VkDevice device){
//     // pipelines.debug.DestroyLayout(device);
//     // pipelines.debug.DestroyPipeline(device);

//     vkDestroyPipeline(device, pipelines.offscreen, nullptr);
// }
// void PickingTexture::CreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout, const VkPipelineCache cache, VkVertexInputBindingDescription bindingDescriptions, const VkVertexInputAttributeDescription *pAttributeDescriptions, uint32_t count){
//     VkPipelineVertexInputStateCreateInfo inputState{};
//     std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;

//     VkPipelineViewportStateCreateInfo viewportState = Pipeline::initializers::pipelineViewportStateCreateInfo(1, 1, 0);
//     VkGraphicsPipelineCreateInfo pipelineCreateInfo = Pipeline::initializers::pipelineCreateInfo(layout, offscreenPass.renderPass, 0);
//     VkPipelineColorBlendAttachmentState blendAttachmentState = Pipeline::initializers::pipelineColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);
//     VkPipelineColorBlendStateCreateInfo colorBlendState = Pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
//     VkPipelineMultisampleStateCreateInfo multisampleState = Pipeline::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
//     VkPipelineDepthStencilStateCreateInfo depthStencilState = Pipeline::initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
//     VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = Pipeline::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
//     VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
//     // std::array<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
//     // VkPipelineDynamicStateCreateInfo dynamicState = Pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);
//     VkRect2D scissors = Pipeline::Rect2D(offscreenPass.width, offscreenPass.height);
//     VkViewport viewport = Pipeline::Viewport(offscreenPass.width, offscreenPass.height);
//     viewportState.pScissors = &scissors;
//     viewportState.pViewports = &viewport;

//     inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
// 	inputState.vertexBindingDescriptionCount = 1;
//     inputState.vertexAttributeDescriptionCount = count;
//     inputState.pVertexBindingDescriptions = &bindingDescriptions;
//     inputState.pVertexAttributeDescriptions = pAttributeDescriptions;
    
//     shaderStages[0] = Pipeline::LoadShader(device, "shaders/base.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
//     shaderStages[1] = Pipeline::LoadShader(device, "shaders/offscreen.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

//     pipelineCreateInfo.pStages = shaderStages.data();
//     pipelineCreateInfo.stageCount = shaderStages.size();
//     pipelineCreateInfo.pVertexInputState = &inputState;
//     pipelineCreateInfo.pViewportState = &viewportState;
//     // pipelineCreateInfo.pDynamicState = &dynamicState;
//     pipelineCreateInfo.pColorBlendState = &colorBlendState;
//     pipelineCreateInfo.pMultisampleState = &multisampleState;
//     pipelineCreateInfo.pDepthStencilState = &depthStencilState;
//     pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
//     pipelineCreateInfo.pRasterizationState = &rasterizationState;
//     VK_CHECK(vkCreateGraphicsPipelines(device, cache, 1, &pipelineCreateInfo, nullptr, &pipelines.offscreen));
//     vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
//     vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
// }
// void PickingTexture::DestroyOffscreenFramebuffer(VkDevice device){
//     DestroyOffscreen(device);
//     offscreenPass.color.Destroy(device);
//     offscreenPass.depth.Destroy(device);
//     vkDestroyFramebuffer(device, offscreenPass.frameBuffer, nullptr);
// }

// PixelInfo PickingTexture::ReadPixel(VkDevice device, double xpos, double ypos){
//     PixelInfo info;
//     // Map image memory so we can start copying from it
//     const char* data;
//     vkMapMemory(device, mCopyColorImage.memory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
//     data += subResourceLayout.offset;
//     // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
//     // ppm binary pixel data
//     //目前没看出问题...或许算是完成了
//     for (glm::float32_t y = 0; y < offscreenPass.height; ++y){
//         glm::float32_t *pixel = (glm::float32_t*)data;
//         for (glm::float32_t x = 0; x < offscreenPass.width; ++x){
//             // printf("%d", val);
//             if(x == xpos && y == ypos){
//                 memcpy(&info, pixel, sizeof(PixelInfo));
//                 y = offscreenPass.height;
//                 break;
//             }
//             pixel += 4;
//         }
//         // printf("\n");
//         data += subResourceLayout.rowPitch;
//     }
//     // Clean up resources
//     vkUnmapMemory(device, mCopyColorImage.memory);
//     return info;
// }
// PixelInfo PickingTexture::ReadPixel(VkDevice device, double xpos, double ypos, VkQueue graphics, VulkanPool pool){
//     UpdateImageData(device, graphics, pool);
//     return ReadPixel(device, xpos, ypos);
// }
// VkResult PickingTexture::Submit(VkQueue graphics, VkSemaphore wait, VkSemaphore &signal){
//     signal = offscreenPass.semaphore;
//     return vulkanFrame::Submit(mCommand, graphics, wait, offscreenPass.semaphore, nullptr);
// }

// void PickingTexture::UpdateImageData(VkDevice device, VkQueue graphics, VulkanPool pool){
//     // Get layout of the image (including row pitch)
//     VkImageSubresource subResource { mCopyColorImage.subresourceRange.aspectMask, 0, 0 };
//     vkGetImageSubresourceLayout(device, mCopyColorImage.image, &subResource, &subResourceLayout);

//     offscreenPass.color.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//     VkCommandBuffer command = vulkanFrame::BeginSingleTimeCommands(device, pool);
//     mCopyColorImage.CopyImage(command, offscreenPass.color);
//     vulkanFrame::EndSingleTimeCommands(device, pool.command, graphics, command);
// }