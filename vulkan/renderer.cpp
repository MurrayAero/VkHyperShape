#include "pool.hpp"
#include "renderer.hpp"
namespace vulkan{
    void Synchronize::Destroy(vk::Device device){
        device.destroyFence(fences);
        device.destroySemaphore(imageAcquired);
        device.destroySemaphore(renderComplete);
        // for (size_t i = 0; i < fences.size(); ++i){
        //     device.destroyFence(fences[i]);
        //     device.destroySemaphore(imageAcquired[i]);
        //     device.destroySemaphore(renderComplete[i]);
        // }
    }
    void Synchronize::Create(vk::Device device){
        vk::FenceCreateInfo finfo = {};
        finfo.sType = vk::StructureType::eFenceCreateInfo;
        finfo.flags = vk::FenceCreateFlagBits::eSignaled;
        vk::SemaphoreCreateInfo info = {};
        info.sType = vk::StructureType::eSemaphoreCreateInfo;
        fences = device.createFence(finfo);
        imageAcquired = device.createSemaphore(info);
        renderComplete = device.createSemaphore(info);
        // fences.resize(count);
        // imageAcquired.resize(count);
        // renderComplete.resize(count);
        // for (size_t i = 0; i < count; ++i) {
        // 	fences[i] = device.createFence(finfo);
        // 	imageAcquired[i] = device.createSemaphore(info);
        // 	renderComplete[i] = device.createSemaphore(info);
        // }
    }
    void RenderEngine::Cleanup(){
        vk::Device dev = *mDevice;
        dev.destroyRenderPass(renderPass);
        for (auto&it:synchronize){
            it.Destroy(*mDevice);
        }
        CleanupSwapchain();
        auto instance = mDevice->GetInstance();
        instance.destroySurfaceKHR(surface);
    }
    RenderEngine::RenderEngine(){
    }
    RenderEngine::~RenderEngine(){
        
    }
    void RenderEngine::CleanupSwapchain(){
        vk::Device dev = *mDevice;
        depthStencil.Destroy(*mDevice);
        for (auto&it:framebuffers){
            dev.destroyFramebuffer(it);
        }
        swapchain.Cleanup(*mDevice);
    }
    void RenderEngine::CreateSwapchain(uint32_t width, uint32_t height){
        swapchain.Create(*mDevice, surface, width, height);
    }
    void RenderEngine::CreateDepthImage(){
        vk::Extent3D size;
        auto swapchainExtent = GetSwapchainSize();
        size.depth = 1;
        size.width = swapchainExtent.width;
        size.height = swapchainExtent.height;
        depthStencil.SetAspectMask(vk::ImageAspectFlagBits::eDepth|vk::ImageAspectFlagBits::eStencil);
        depthStencil.Create(*mDevice, size, vk::ImageUsageFlagBits::eDepthStencilAttachment, DEPTH_FORMAT);
        depthStencil.CreateView(*mDevice);
    }
    void RenderEngine::CreateFrameBuffer(){
        auto swapchainExtent = GetSwapchainSize();
        framebuffers.resize(swapchain.GetImageSize());
        std::array<vk::ImageView, 2>frameBufferAttachments;

        frameBufferAttachments[1] = depthStencil.GetView();

        vk::FramebufferCreateInfo info = {};
        info.sType = vk::StructureType::eFramebufferCreateInfo;
        info.layers = 1;
        info.renderPass = renderPass;
        info.width = swapchainExtent.width;
        info.height = swapchainExtent.height;
        info.pAttachments = frameBufferAttachments.data();
        info.attachmentCount = frameBufferAttachments.size();
        vk::Device dev = *mDevice;
        for (size_t i = 0; i < framebuffers.size(); ++i){
            frameBufferAttachments[0] = swapchain.GetImage(i).GetView();
            framebuffers[i] = dev.createFramebuffer(info);
        }
    }
    void RenderEngine::CreateSynchronize(vk::Device device){
        synchronize.resize(swapchain.GetImageSize());
        for (auto&it:synchronize){
            it.Create(device);
        }
    }
    // void RenderEngine::CreateRenderPass(vk::Device device) {
    //     std::array<vk::SubpassDependency2, 1> dependencies = {};
    //     dependencies[0].sType = vk::StructureType::eSubpassDependency2;
    //     dependencies[0].dstSubpass = 0;
    //     dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    //     dependencies[0].srcAccessMask = vk::AccessFlagBits::eNoneKHR;
    //     dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //     dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //     dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

    //     vk::AttachmentReference2 colorAttachmentRef{};
    //     colorAttachmentRef.sType = vk::StructureType::eAttachmentReference2;
    //     colorAttachmentRef.attachment = 0;
    //     colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    //     vk::AttachmentReference2 depthAttachmentRef{};
    //     depthAttachmentRef.sType = vk::StructureType::eAttachmentReference2;
    //     depthAttachmentRef.attachment = 1;
    //     depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    //     std::array<vk::SubpassDescription2, 1> subpasses = {};
    //     subpasses[0].sType = vk::StructureType::eSubpassDescription2;
    //     subpasses[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    //     subpasses[0].colorAttachmentCount = 1;
    //     subpasses[0].pColorAttachments = &colorAttachmentRef;
    //     subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;

    //     std::array<vk::AttachmentDescription2, 2> attachments = {};
        
    //     attachments[0].sType = vk::StructureType::eAttachmentDescription2;
    //     attachments[0].format = swapchain.surfaceFormat.format;
    //     attachments[0].samples = vk::SampleCountFlagBits::e1;
    //     attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
    //     attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
    //     attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    //     attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    //     attachments[0].initialLayout = vk::ImageLayout::eUndefined;
    //     attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

    //     attachments[1].sType = vk::StructureType::eAttachmentDescription2;
    //     attachments[1].format = DEPTH_FORMAT;
    //     attachments[1].samples = vk::SampleCountFlagBits::e1;
    //     attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
    //     attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
    //     attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
    //     attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    //     attachments[1].initialLayout = vk::ImageLayout::eUndefined;  // 显式设置初始布局
    //     attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    //     vk::RenderPassCreateInfo2 createInfo{};
    //     createInfo.sType = vk::StructureType::eRenderPassCreateInfo2;
    //     createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    //     createInfo.pAttachments = attachments.data();
    //     createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    //     createInfo.pSubpasses = subpasses.data();
    //     createInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    //     createInfo.pDependencies = dependencies.data();

    //     renderPass = device.createRenderPass2(createInfo);
    // }
    void RenderEngine::CreateRenderPass(vk::Device device){
        std::array<vk::SubpassDependency, 1> dependency = {};
        dependency[0].setDstSubpass(0);
        dependency[0].setSrcSubpass(VK_SUBPASS_EXTERNAL);
        dependency[0].setSrcAccessMask(vk::AccessFlagBits::eNoneKHR);
        dependency[0].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        dependency[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

        vk::AttachmentReference colorAttachmentRef;
        vk::AttachmentReference depthAttachmentRef;
        colorAttachmentRef.attachment = 0;//framebuffer中第一个imageview
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
        depthAttachmentRef.attachment = 1;//framebuffer中第二个imageview
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        
        std::array<vk::SubpassDescription, 1>subPass = {};
        subPass[0].colorAttachmentCount = 1;
        subPass[0].pColorAttachments = &colorAttachmentRef;
        subPass[0].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subPass[0].pDepthStencilAttachment = &depthAttachmentRef;
        // subPass[0].pInputAttachments = VK_NULL_HANDLE;//被着色器读取的附着
        // subPass[0].pResolveAttachments =  VK_NULL_HANDLE;//用于多重采样的颜色附着
        // subPass[0].pPreserveAttachments = VK_NULL_HANDLE;//保留的附着, 用于在子流程之间传递数据
        
        std::array<vk::AttachmentDescription, 2>attachmentDescription = {};
        attachmentDescription[0].format = swapchain.GetSurfaceFormat();
        attachmentDescription[0].samples = vk::SampleCountFlagBits::e1;
        attachmentDescription[0].loadOp = vk::AttachmentLoadOp::eClear;
        attachmentDescription[0].storeOp = vk::AttachmentStoreOp::eStore;
        // attachmentDescription[0].initialLayout = vk::ImageLayout::eUndefined;
        attachmentDescription[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        attachmentDescription[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachmentDescription[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

        attachmentDescription[1].format = DEPTH_FORMAT;
        attachmentDescription[1].samples = vk::SampleCountFlagBits::e1;
        attachmentDescription[1].loadOp = vk::AttachmentLoadOp::eClear;
        // attachmentDescription[1].initialLayout = vk::ImageLayout::eUndefined;
        attachmentDescription[1].storeOp = vk::AttachmentStoreOp::eDontCare;
        attachmentDescription[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
        attachmentDescription[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        attachmentDescription[1].finalLayout = depthAttachmentRef.layout;
        
        vk::RenderPassCreateInfo info = {};
        info.sType = vk::StructureType::eRenderPassCreateInfo;
        info.pSubpasses = subPass.data();
        info.subpassCount = subPass.size();
        info.pDependencies = dependency.data();
        info.dependencyCount = dependency.size();
        info.pAttachments = attachmentDescription.data();
        info.attachmentCount = attachmentDescription.size();
        renderPass = device.createRenderPass(info);
    }
    vk::Result RenderEngine::Present(uint32_t imageIndex, const vk::Queue present, const vk::Semaphore&renderComplete){
        vk::SwapchainKHR swapchain = this->swapchain;
        vk::PresentInfoKHR presentInfo = {};
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderComplete;
        return present.presentKHR(presentInfo);
    }
    void RenderEngine::DynamicRendering(vk::Device device, const Queue &vulkanQueue, void (*recordCommand)(vk::CommandBuffer, vulkan::Image &, vulkan::Image &), void (*recreateSwapchain)(void *userData), void *userData){
        VK_CHECK_LOG(device.waitForFences(synchronize[currentFrame].fences, VK_TRUE, UINT64_MAX));
        device.resetFences(synchronize[currentFrame].fences);
        if(threadCount > 0){
            std::vector<std::thread>thread(threadCount);
            for (uint32_t i = 0; i < threadCount; ++i){
                thread[i] = std::move(std::move(std::thread(threadFun, secondaryCommandBuffers[currentFrame][i], i)));
            }
            for (uint32_t i = 0; i < threadCount; ++i){
                thread[i].join();//不应该让主线程空闲
            }
        }
        uint32_t imageIndex;
        vk::Result result = device.acquireNextImageKHR(swapchain, UINT64_MAX, synchronize[currentFrame].imageAcquired, VK_NULL_HANDLE, &imageIndex);
        if (recreateSwapchain && (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)){
            spdlog::info("in function {}:Recreating swapchain", __FUNCTION__);
            recreateSwapchain(userData);
            for (auto&it:synchronize){
                it.Destroy(device);
                it.Create(device);
            }
            currentFrame = 0;
            return;
        }
        else if(result != vk::Result::eSuccess){
            VK_CHECK_LOG(result);
        }
        recordCommand(commandBuffers[currentFrame], swapchain.GetImage(imageIndex), depthStencil);
        Submit(commandBuffers[currentFrame], vulkanQueue.graphics, synchronize[currentFrame].imageAcquired, synchronize[currentFrame].renderComplete, synchronize[currentFrame].fences);
        result = Present(imageIndex, vulkanQueue.present, synchronize[currentFrame].renderComplete);
        if (recreateSwapchain && (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)) {
            spdlog::info("in function {}:Swapchain recreated", __FUNCTION__);
            recreateSwapchain(userData);
            return;
        }
        else if(vk::Result::eSuccess != result){
            VK_CHECK_LOG(result);
        }
        currentFrame = (currentFrame + 1) % swapchain.GetImageSize();
    }
    void RenderEngine::Render(vk::Device device, const Queue &vulkanQueue, void (*recordCommand)(vk::CommandBuffer, vk::Framebuffer, vk::RenderPass), void (*recreateSwapchain)(void *userData), void *userData){
        VK_CHECK_LOG(device.waitForFences(synchronize[currentFrame].fences, VK_TRUE, UINT64_MAX));
        device.resetFences(synchronize[currentFrame].fences);
        if(threadCount > 0){
            std::vector<std::thread>thread(threadCount);
            for (uint32_t i = 0; i < threadCount; ++i){
                thread[i] = std::move(std::move(std::thread(threadFun, secondaryCommandBuffers[currentFrame][i], i)));
            }
            for (uint32_t i = 0; i < threadCount; ++i){
                thread[i].join();//不应该让主线程空闲
            }
        }
        uint32_t imageIndex;
        vk::Result result = device.acquireNextImageKHR(swapchain, UINT64_MAX, synchronize[currentFrame].imageAcquired, VK_NULL_HANDLE, &imageIndex);
        if (recreateSwapchain && (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)){
            spdlog::info("in function {}:Swapchain recreated", __FUNCTION__);
            recreateSwapchain(userData);
            for (auto&it:synchronize){
                it.Destroy(device);
                it.Create(device);
            }
            currentFrame = 0;
            return;
        }
        else if(result != vk::Result::eSuccess){
            VK_CHECK_LOG(result);
        }
        recordCommand(commandBuffers[currentFrame], framebuffers[currentFrame], renderPass);
        Submit(commandBuffers[currentFrame], vulkanQueue.graphics, synchronize[currentFrame].imageAcquired, synchronize[currentFrame].renderComplete, synchronize[currentFrame].fences);
        result = Present(imageIndex, vulkanQueue.present, synchronize[currentFrame].renderComplete);
        if (recreateSwapchain && (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)) {
            spdlog::info("in function {}:Swapchain recreated", __FUNCTION__);
            recreateSwapchain(userData);
            return;
        }
        else if(vk::Result::eSuccess != result){
            VK_CHECK_LOG(result);
        }
        currentFrame = (currentFrame + 1) % swapchain.GetImageSize();
    }
    void RenderEngine::Submit(const vk::CommandBuffer&commandbuffers, vk::Queue graphics, const vk::Semaphore&imageAcquired, const vk::Semaphore&renderComplete, const vk::Fence&fence){
        vk::SubmitInfo submitInfo = {};
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandbuffers;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAcquired;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderComplete;
        submitInfo.pWaitDstStageMask = waitStages;
        graphics.submit(submitInfo, fence);
    }

    void RenderEngine::Setup(const Device &device, uint32_t width, uint32_t height, const vulkan::Pool&pool){
        mDevice = &device;
        CreateSwapchain(width, height);
        CreateDepthImage();
        if(!device.IsEnableDynamicRendering()){
            CreateRenderPass(device);
            CreateFrameBuffer();
        }
        CreateSynchronize(device);
        commandBuffers = pool.AllocateCommandBuffers(device, swapchain.GetImageSize());
    }
    void RenderEngine::SetThreadFun(void(*threadFun)(vk::CommandBuffer, uint32_t), uint32_t threadCount)noexcept{
        if(threadCount > 0){
            this->threadFun = threadFun;
            this->threadCount = threadCount;
            commandPool.resize(threadCount);
            secondaryCommandBuffers.resize(swapchain.GetImageSize());
            vk::Device device = *mDevice;
            for (size_t i = 0; i < threadCount; i++){
                commandPool[i] = device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, mDevice->GetQueueFamiliesIndex(vk::QueueFlagBits::eGraphics)));                
                // secondaryCommandBuffers[i] = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool[i], vk::CommandBufferLevel::eSecondary, swapchain.GetImageSize()));
            }
            for (size_t i = 0; i < swapchain.GetImageSize(); i++){
                secondaryCommandBuffers[i].resize(threadCount);
                for (size_t j = 0; j < threadCount; j++){
                    secondaryCommandBuffers[i][j] = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool[j], vk::CommandBufferLevel::eSecondary, 1))[0];
                }
            }
        }
    }
}