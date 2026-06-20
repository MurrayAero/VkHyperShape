#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "camera.h"
#include "VulkanDemo.h"
#include "VulkanWindow.h"
#include "PickingTexture.h"
// #include <xcb/xcb.h>
// #include <vulkan/vulkan_xcb.h>
struct PushConstant{
    // glm::mat4 model;
    // glm::mat4 projection;
};
struct GraphicsPipeline{
    VkPipeline pipeline;
    //整个程序只需要一个
    VkPipelineCache cache;
    VkPipelineLayout layout;
};
VulkanPool g_VulkanPool;
VulkanQueue g_VulkanQueue;
VulkanDevice g_VulkanDevice;
VulkanWindow g_VulkanRenderer;
VulkanSynchronize g_VulkanSynchronize;

uint32_t g_WindowWidth = 700, g_WindowHeight = g_WindowWidth;


Camera g_Camera;
VulkanDemo g_Demo;

GraphicsPipeline g_Pipeline;
#ifdef USE_VULKAN_HPP
vk::DescriptorSetLayout g_SetLayout;
vk::CommandBuffer g_CommandBuffers;
#else
VkDescriptorSetLayout g_SetLayout;
VkCommandBuffer g_CommandBuffers;
#endif
PickingTexture g_PickingTexture;

glm::vec3 g_Target[2];
//拾取类现在不支持多管线
void Draw(VkCommandBuffer command, const VkPipeline *pipeline, uint32_t count){
    PushConstant pc;
    const glm::mat4 scale = glm::scale(glm::mat4(1), glm::vec3(.15));
    pc.projection = g_Camera.GetPerspective(g_WindowWidth / g_WindowHeight);
    pc.projection[1][1] *= -1;
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline[0]);
    g_Target[0] = glm::vec3(0);
    g_Target[1] = g_Camera.GetCenter();
    for (size_t i = 0; i < 2; ++i){
        pc.model = glm::translate(glm::mat4(1), g_Target[i]) * scale;
        vkCmdPushConstants(command, g_Pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant), &pc);
        g_Demo.Draw(command, g_Pipeline.layout);       
    }    
}
void RecordCommand(VkCommandBuffer command, VkFramebuffer frame){
    vulkanFrame::BeginCommands(command, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    g_PickingTexture.DrawToOffscreen(command, Draw);
    vulkanFrame::BeginRenderPass(command, frame, g_VulkanWindow.renderPass, g_WindowWidth, g_WindowHeight);

    Draw(command, &g_Pipeline.pipeline, 1);

    vkCmdEndRenderPass(command);
    vkEndCommandBuffer(command);
}
void keybutton(GLFWwindow *window, int key, int scancode, int action, int mods){
    // float angle = 10;
    if(action == GLFW_PRESS){
        if(key == GLFW_KEY_ESCAPE){
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        glm::vec3 front = glm::normalize(g_Camera.GetCenter() - g_Camera.GetPosition());
        switch (key){
            case GLFW_KEY_W:
                front.y = 0;
                g_Camera.MoveCenter(front *.25f);
            break;
            default:
            break;
        }
        g_Camera.UpdateUniform(g_VulkanDevice.device);
    }
}
void mousescroll(GLFWwindow *window, double xoffset, double yoffset){;
    g_Camera.ProcessMouseScroll(yoffset);
}
glm::vec2 g_MousePos;
void mousecursor(GLFWwindow *window, double xpos, double ypos){
    static const float sensitivity = 0.1f;
    glm::vec2 currentMouse = glm::vec2(xpos, ypos);

    // 计算鼠标位移（注意Y轴反转）
    glm::vec2 offset = currentMouse - g_MousePos;
    
    g_MousePos = currentMouse;

    // 应用灵敏度并转换角度
    offset *= sensitivity;
    
    // 传递给更新函数
    g_Camera.UpdateRotation(offset.x * 0.01f, offset.y * 0.01f);
    g_Camera.UpdateUniform(g_VulkanDevice.device);
}
void mousebutton(GLFWwindow *window,int button,int action,int mods){
    // Ray ray;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (action == GLFW_PRESS){
        g_MousePos = glm::vec2(xpos, ypos);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } 
    // if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
    //     ray = CalcPickingRay(g_MousePos);
    //     glm::mat4 view = g_Camera.GetView();
    //     TransformRay(ray, glm::inverse(view));
    //     BoundingSphere sphere{glm::vec3(0), .5f};
    //     if(RaySphereIntTest(ray, sphere)){
    //         printf("选中立方体\n");
    //     }
    // }
    // if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT){
    //     PixelInfo info = g_PickingTexture.ReadPixel(g_VulkanDevice.device, xpos, ypos, g_VulkanQueue.graphics, g_VulkanPool);
    //     if(info.outColor.x != PICKING_TEXTURE_CLEAR_SCREEN_VAL){
    //         printf("选中立方体\n");
    //     }
    //     else{
    //         printf("未选中立方体\n");
    //     }
    // }
}
void DestroyGraphicsPipeline(VkDevice device){
    vkDestroyPipeline(device, g_Pipeline.pipeline, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(device, g_Pipeline.layout, VK_NULL_HANDLE);
}
void CreatePipelineLayout(VkDevice device, VkDescriptorSetLayout layout){
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = Pipeline::initializers::pipelineLayoutCreateInfo(1, 1);
    VkPushConstantRange pushConstantRange = Pipeline::initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstant), 0);
    pipelineLayoutInfo.pSetLayouts = &layout;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &g_Pipeline.layout));
}
void CreateGraphicsPipeline(VkDevice device, VkPipelineLayout layout){
    // state.mColorBlend.blendEnable = VK_TRUE;
    // state.mColorBlend.alphaBlendOp = VK_BLEND_OP_ADD;
    // state.mColorBlend.colorBlendOp = VK_BLEND_OP_ADD;
    // //该混合方式, 不依赖着色器输出的颜色决定透明度, 但必须需要2个片段以上才看得出来
    // state.mColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
    // state.mColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
    // state.mColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    // state.mColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    // //该混合方式, 依赖着色器输出颜色中的a(w)值决定透明度, 1个片段也能看出来
    // // state.mColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // // state.mColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // // state.mColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // // state.mColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    //imgui绘制字体时的混合方式
    // state.mColorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // state.mColorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // state.mColorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    // state.mColorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;//VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

    VkPipelineVertexInputStateCreateInfo inputState{};
    std::array<VkPipelineShaderStageCreateInfo,2> shaderStages;
    auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
    VkVertexInputBindingDescription bindingDescriptions = Vertex::inputBindingDescription(0);

    VkPipelineViewportStateCreateInfo viewportState = Pipeline::initializers::pipelineViewportStateCreateInfo(1, 1, 0);
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = Pipeline::initializers::pipelineCreateInfo(layout, g_VulkanWindow.renderPass, 0);
    VkPipelineColorBlendAttachmentState blendAttachmentState = Pipeline::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
    VkPipelineColorBlendStateCreateInfo colorBlendState = Pipeline::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
    VkPipelineMultisampleStateCreateInfo multisampleState = Pipeline::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
    VkPipelineDepthStencilStateCreateInfo depthStencilState = Pipeline::initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = Pipeline::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
    VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
    // VkPipelineRasterizationStateCreateInfo rasterizationState = Pipeline::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
    // std::array<VkDynamicState, 2> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    // VkPipelineDynamicStateCreateInfo dynamicState = Pipeline::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), static_cast<uint32_t>(dynamicStateEnables.size()), 0);
    VkRect2D scissors = Pipeline::Rect2D(g_WindowWidth, g_WindowHeight);
    VkViewport viewport = Pipeline::Viewport(g_WindowWidth, g_WindowHeight);
    viewportState.pScissors = &scissors;
    viewportState.pViewports = &viewport;

    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.vertexBindingDescriptionCount = 1;
    inputState.pVertexBindingDescriptions = &bindingDescriptions;
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();

    shaderStages[0] = Pipeline::LoadShader(device, "shaders/base.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = Pipeline::LoadShader(device, "shaders/base.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    // pipelineCreateInfo.pTessellationState = ;
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.stageCount = shaderStages.size();

    pipelineCreateInfo.pVertexInputState = &inputState;
    pipelineCreateInfo.pViewportState = &viewportState;
    // pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    VK_CHECK(vkCreateGraphicsPipelines(device, g_Pipeline.cache, 1, &pipelineCreateInfo, nullptr, &g_Pipeline.pipeline));
    vkDestroyShaderModule(device, shaderStages[0].module, VK_NULL_HANDLE);
    vkDestroyShaderModule(device, shaderStages[1].module, VK_NULL_HANDLE);
}
void SetupDescriptorSetLayout(VkDevice device){
    VkDescriptorSetLayoutBinding setlayoutBindings[2] = {};
    // setlayoutBinding.binding = 0;
    setlayoutBindings[0].descriptorCount = 1;
    setlayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    setlayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setlayoutBindings[1].binding = 1;
    setlayoutBindings[1].descriptorCount = 1;
    setlayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    setlayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    VK_CHECK(vulkanFrame::CreateDescriptorSetLayout(device, setlayoutBindings, sizeof(setlayoutBindings) / sizeof(VkDescriptorSetLayoutBinding), &g_SetLayout));
}
//void FramebufferResizeCallback(GLFWwindow* window, int width, int height){
//     g_WindowResize = true;
// }
void Setup(GLFWwindow *window){
    glfwSetKeyCallback(window, keybutton);
    glfwSetScrollCallback(window, mousescroll);
    glfwSetCursorPosCallback(window, mousecursor);
    glfwSetMouseButtonCallback(window, mousebutton);

    // glfwSetFramebufferSizeCallback( window, FramebufferResizeCallback);

    SetupDescriptorSetLayout(g_VulkanDevice.device);
    
    Pipeline::CreatePipelineCache(g_VulkanDevice.device, "GraphicsPipelineCache", g_Pipeline.cache);
    CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
    CreateGraphicsPipeline(g_VulkanDevice.device, g_Pipeline.layout);

    g_Camera.CreateUniform(g_VulkanDevice);
    //可以考虑自定义front
    g_Camera.SetCenter(glm::vec3(0, 0, 5));
    g_Camera.UpdateUniform(g_VulkanDevice.device);

    g_Demo.Setup(g_VulkanDevice, g_SetLayout, g_VulkanQueue.graphics, g_VulkanPool);
    g_Demo.UpdateSet(g_VulkanDevice.device, g_Camera.uniform);


    g_PickingTexture.Setup(g_VulkanDevice, g_VulkanPool, g_WindowWidth, g_WindowHeight);
    auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
    g_PickingTexture.CreateGraphicsPipeline(g_VulkanDevice.device, g_Pipeline.layout, g_Pipeline.cache, Vertex::inputBindingDescription(0), attributeDescriptions.data(), attributeDescriptions.size());

    g_VulkanPool.AllocateCommandBuffers(g_VulkanDevice.device, 1, &g_CommandBuffers);
}
void RecreateSwapchain(void *userData){
    GLFWwindow* window = (GLFWwindow*)userData;
    int32_t width, height;
    do{
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }while(width == 0 || height == 0);
    vkDeviceWaitIdle(g_VulkanDevice.device);
    //想不调用vkDeviceWaitIdle重建交换链也行, 在重新创建交换链时, VkSwapchainCreateInfoKHR的结构体中oldSwapChain设置为原来的交换链
    g_VulkanRenderer.CleanupSwapchain(g_VulkanDevice.device);
   const VkExtent2D swapchainExtent = g_VulkanRenderer.swapchain.extent;
   g_VulkanRenderer.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanRenderer.surface);
    g_WindowWidth = g_VulkanRenderer.swapchain.extent.width;
    g_WindowHeight = g_VulkanRenderer.swapchain.extent.height;
    g_VulkanRenderer.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanRenderer.CreateFrameBuffer(g_VulkanDevice);    
    if(swapchainExtent.width != g_WindowWidth || swapchainExtent.height != g_WindowHeight){
        DestroyGraphicsPipeline(g_VulkanDevice.device);
        CreatePipelineLayout(g_VulkanDevice.device, g_SetLayout);
        CreateGraphicsPipeline(g_VulkanDevice.device, g_Pipeline.layout);

        //等到能正确创建交换链后, 再重建帧缓冲
        // g_PickingTexture.CreateOffscreen(g_VulkanDevice, g_WindowWidth, g_WindowHeight);
        // g_PickingTexture.DestroyGraphicsPipeline(g_VulkanDevice.device);
        // auto attributeDescriptions = Vertex::inputAttributeDescriptions(0);
        // g_PickingTexture.CreateGraphicsPipeline(g_VulkanDevice.device, g_Pipeline.layout, g_Pipeline.cache, Vertex::inputBindingDescription(0), attributeDescriptions.data(), attributeDescriptions.size());
    }
}
void display(GLFWwindow* window){
    static size_t currentFrame;
    vkDeviceWaitIdle(g_VulkanDevice.device);
    RecordCommand(g_CommandBuffers, g_VulkanRenderer.framebuffers[currentFrame]);
    vulkanFrame::Render(g_VulkanDevice.device, currentFrame, g_CommandBuffers, g_VulkanWindow.swapchain.swapchain, g_VulkanQueue, g_VulkanSynchronize, RecreateSwapchain, window);

    currentFrame = (currentFrame + 1) % g_VulkanRenderer.framebuffers.size();
}
void Cleanup(VkDevice device){
    //如果验证层提示某对象无销毁，并且一直找不到，可以在VulkanBuffer类的CreraterBuffer或类似的创建函数打印信息查找
    vkDestroyDescriptorSetLayout(device, g_SetLayout, VK_NULL_HANDLE);
    DestroyGraphicsPipeline(device);
    Pipeline::DestroyPipelineCache(device, "GraphicsPipelineCache", g_Pipeline.cache);
    g_Demo.Cleanup(device);
    g_Camera.DestroyUniform(device);
    g_PickingTexture.Cleanup(device);
}
bool GetPhysicalDevices(VkPhysicalDevice physicalDevice){
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    return physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}
void SetupVulkan(GLFWwindow *window){
    uint32_t count;
    const char** instanceExtension = glfwGetRequiredInstanceExtensions(&count);
    std::vector<const char*> extensions(instanceExtension, instanceExtension + count);
#ifdef DEBUG
    g_VulkanDevice.EnableValidation();
#endif
    VK_CHECK(g_VulkanDevice.CreateInstance(extensions));
    g_VulkanDevice.SelectPhysicalDevices(GetPhysicalDevices);
    glfwCreateWindowSurface(g_VulkanDevice.instance, window, nullptr, &g_VulkanRenderer.surface);
    VK_CHECK(g_VulkanDevice.CreateDevice(g_VulkanRenderer.surface));
    g_VulkanQueue.CreateQueue(g_VulkanDevice, g_VulkanRenderer.surface);
    g_VulkanRenderer.swapchain.CreateSwapchain(g_VulkanDevice, g_VulkanRenderer.surface);
    g_VulkanRenderer.CreateRenderPass(g_VulkanDevice.device);
    g_VulkanRenderer.CreateFrameBuffer(g_VulkanDevice);
    g_VulkanPool.CreatePool(g_VulkanDevice, 2);
    g_VulkanSynchronize.CreateSynchronize(g_VulkanDevice.device, g_VulkanRenderer.swapchain.images.size());
    //显示设备信息
    const char *deviceType;
    VkPhysicalDeviceProperties physicalDeviceProperties = g_VulkanDevice.GetPhysicalDeviceProperties();
    switch (physicalDeviceProperties.deviceType){
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        deviceType = "CPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        deviceType = "DISCRETE GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        deviceType = "INTEGRATED GPU";
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        deviceType = "VIRTUAL GPU";
        break;
    default:
        deviceType = "OTHER";
        break;
    }
	printf("gpu name:%s, gpu type:%s\n", physicalDeviceProperties.deviceName, deviceType);
    // PrintVulkanInfo(&physicalDeviceProperties);
}
void CleanupVulkan(){
    g_VulkanSynchronize.Cleanup(g_VulkanDevice.device);

    g_VulkanPool.Cleanup(g_VulkanDevice.device);

    g_VulkanRenderer.Cleanup(g_VulkanDevice);

    g_VulkanDevice.Cleanup();
}
int main(){
    if (GLFW_FALSE == glfwInit()) {
        printf("initialize glfw error");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "demo", NULL, NULL);
    // Setup Vulkan
    if (!glfwVulkanSupported()){
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    SetupVulkan(window);
    Setup(window);

    const double targetFrameTime = 1.0 / 60.0; // 目标帧时间（例如60 FPS）
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime - previousTime).count();

        if (elapsedTime < targetFrameTime) {
            // 计算剩余需要等待的时间
            double sleepTime = (targetFrameTime - elapsedTime) * 1000; // 转换为毫秒
            std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepTime));
        }

        // 更新前一帧时间
        previousTime = std::chrono::high_resolution_clock::now();

        display(window);

        glfwPollEvents();
    }
	glfwTerminate();

    vkDeviceWaitIdle(g_VulkanDevice.device);
    Cleanup(g_VulkanDevice.device);
    CleanupVulkan();
    return 0;
}