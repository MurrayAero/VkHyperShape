#include "pipeline.hpp"
#include "framework.hpp"
uint32_t vulkan::pipeline::tools::GetFileSize(FILE *fp){
    uint32_t size = 0;
    if(fp){
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    }
    return size;
}
// template<typename DATATYPE>
// uint32_t vulkan::pipeline::tools::GetFileContent(const std::string&file, std::vector<DATATYPE>&data){
//     FILE *fp = fopen(file.c_str(), "rb");
//     if(!fp){
//         perror("open file error");
//         printf("file is %s\n", file.c_str());
//         return 0;
//     }
//     uint32_t size = GetFileSize(fp);
//     data.resize(size / sizeof(DATATYPE));
//     fread(data.data(), size, 1, fp);
//     fclose(fp);
//     return size;
// }
bool vulkan::pipeline::tools::WriteFileContent(const std::string &file, const void *data, uint32_t size){
    if(data == nullptr){
		printf("write file content error:data is nullptr\n");
        spdlog::error("in function {}:write file content error:data is nullptr", __FUNCTION__);
        return false;
    }
    FILE *fp = fopen(file.c_str(), "wb");
    if(!fp){
		spdlog::error("in function {}:open file error,file name is {}", __FUNCTION__, file.c_str());
        perror("open file error");
        printf("in function %s: file is %s\n", __FUNCTION__, file.c_str());
        return false;
    }
    fwrite(data, size, 1, fp);
    fclose(fp);
    return true;
}

vk::PipelineShaderStageCreateInfo vulkan::pipeline::tools::loadShader(vk::Device device, const char *filename, vk::ShaderStageFlagBits stage){
    std::vector<uint32_t>code;
    GetFileContent(filename, code);
    return loadShader(device, stage, code.data(), code.size() * sizeof(uint32_t));
}

vk::PipelineShaderStageCreateInfo vulkan::pipeline::tools::loadShader(vk::Device device, vk::ShaderStageFlagBits stage, const uint32_t *code, uint32_t size){
    VkShaderModuleCreateInfo shaderModuleCreateInfo = initializers::shaderModuleCreateInfo(size);
    shaderModuleCreateInfo.pCode = code;
    vk::PipelineShaderStageCreateInfo info {};
    info.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
    info.pName = "main";
    info.stage = stage;
    info.module = device.createShaderModule(shaderModuleCreateInfo);
    return info;
}
vk::PipelineCache vulkan::pipeline::tools::createPipelineCache(vk::Device device, const std::string &file){
    std::vector<uint32_t>cacheData;
    vk::PipelineCacheCreateInfo cacheInfo = {};
    cacheInfo.sType = vk::StructureType::ePipelineCacheCreateInfo;
    if(GetFileContent(file.c_str(), cacheData)){
        cacheInfo.initialDataSize = cacheData.size() * sizeof(uint32_t);
        cacheInfo.pInitialData = cacheData.data();
    }
    return device.createPipelineCache(cacheInfo);
}
void vulkan::pipeline::tools::destroyPipelineCache(vk::Device device, const std::string &file, vk::PipelineCache cache){
    if(file != ""){
        size_t cacheDataSize;
        std::vector<uint32_t>cacheData;
        vkGetPipelineCacheData(device, cache, &cacheDataSize, nullptr);
        cacheData.resize(cacheDataSize / sizeof(char));
        vkGetPipelineCacheData(device, cache, &cacheDataSize, cacheData.data());
        WriteFileContent(file, cacheData.data(), cacheData.size() * sizeof(uint32_t));
    }
    if(cache != VK_NULL_HANDLE)device.destroyPipelineCache(cache);
}
vulkan::pipeline::Pipeline::Pipeline(){
}
vulkan::pipeline::Pipeline::~Pipeline(){
}
void vulkan::pipeline::Pipeline::PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const std::string &shaders){
    std::vector<uint32_t>data;
    uint32_t size = tools::GetFileContent<uint32_t>(shaders, data);
    PushShaders(device, stage, data.data(), size);
}
void vulkan::pipeline::Pipeline::PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const uint32_t *code, uint32_t size){
    PushShaders(device, stage, code, size, nullptr, nullptr);
}
void vulkan::pipeline::Pipeline::PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const std::string &shaders, const vk::SpecializationMapEntry *pSpecializationMapEntry, const void *pData){
    std::vector<uint32_t>data;
    uint32_t size = tools::GetFileContent<uint32_t>(shaders, data);
    PushShaders(device, stage, data.data(), size, pSpecializationMapEntry, pData);
}
void vulkan::pipeline::Pipeline::PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const uint32_t *code, uint32_t size, const vk::SpecializationMapEntry *pSpecializationMapEntry, const void *pData){
    vk::SpecializationInfo specialization = {};
    if(pSpecializationMapEntry){
        specialization.pData = pData;
        specialization.mapEntryCount = 1;
        specialization.dataSize = pSpecializationMapEntry->size;
        specialization.pMapEntries = pSpecializationMapEntry;
    }
    vk::PipelineShaderStageCreateInfo info = tools::loadShader(device, stage, code, size); 
    info.pSpecializationInfo = pSpecializationMapEntry?&specialization:nullptr;
    shaderStages.push_back(info);
}

vulkan::pipeline::Graphics::Graphics(){
}
vulkan::pipeline::Graphics::~Graphics(){
}
void vulkan::pipeline::Graphics::Bind(vk::CommandBuffer command)const noexcept{
    command.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines);
}
bool vulkan::pipeline::Graphics::Create(vk::Device device, vk::PipelineLayout layout, vk::PipelineCache cache){
    if(isWindowSizeInvalid()){
        if(dynamicState.empty() || std::find(dynamicState.begin(), dynamicState.end(), vk::DynamicState::eViewport) == dynamicState.end() ||
            std::find(dynamicState.begin(), dynamicState.end(), vk::DynamicState::eScissor) == dynamicState.end()){
                dynamicState.push_back(vk::DynamicState::eScissor);
                dynamicState.push_back(vk::DynamicState::eViewport);
        }
    }
    vk::GraphicsPipelineCreateInfo info;
    info = initializers::pipelineCreateInfo(layout, renderPass);
    if(shaderStages.empty()){
        spdlog::error("in function {}:shaders is empty", __FUNCTION__);
        Destroy(device);
        return false;
    }
    if(blendAttachmentState.empty())PushColorBlendAttachmentState();
    vk::PipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(blendAttachmentState.size(), blendAttachmentState.data());

    vk::PipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(viewport.empty()?1:viewport.size(), scissor.empty()?1:scissor.size());
    viewportState.pScissors = scissor.data();
    viewportState.pViewports = viewport.data();

    vk::PipelineDynamicStateCreateInfo dynamicStateinfo = initializers::pipelineDynamicStateCreateInfo(dynamicState.data(), static_cast<uint32_t>(dynamicState.size()));

    vk::PipelineVertexInputStateCreateInfo inputState{};
    inputState.sType =  vk::StructureType::ePipelineVertexInputStateCreateInfo;
    inputState.pVertexBindingDescriptions = bindingDescriptions.data();
    inputState.pVertexAttributeDescriptions = attributeDescriptions.data();
    inputState.vertexBindingDescriptionCount = bindingDescriptions.size();
    inputState.vertexAttributeDescriptionCount = attributeDescriptions.size();

    vk::PipelineRenderingCreateInfo renderingInfo;
    if(renderPass == VK_NULL_HANDLE){
        renderingInfo.depthAttachmentFormat = depthFormat;
        renderingInfo.colorAttachmentCount = colorFormat.size();
        renderingInfo.pColorAttachmentFormats = colorFormat.data();
        info.pNext = &renderingInfo;
    }
    info.pStages = shaderStages.data();
    info.stageCount = shaderStages.size();

    info.pVertexInputState = &inputState;
    info.pViewportState = &viewportState;
    if(tessellationState.patchControlPoints > 0){
        info.pTessellationState = &tessellationState;
    }
    if(!dynamicState.empty())info.pDynamicState = &dynamicStateinfo;
    info.pColorBlendState = &colorBlendState;
    info.pMultisampleState = &multisampleState;
    info.pDepthStencilState = &depthStencilState;
    info.pInputAssemblyState = &inputAssemblyState;
    info.pRasterizationState = &rasterizationState;
    auto result = device.createGraphicsPipeline(cache, info);
    if(result.result == vk::Result::eSuccess){
        pipelines = result.value;
        spdlog::debug("in function {}:pipelines:{:#x}", __FUNCTION__, (uintptr_t)(VkPipeline)pipelines);
    }
    else{
        VK_CHECK_LOG(result.result);
        Destroy(device);
        return false;
    }
    for (auto&it:shaderStages){
        device.destroyShaderModule(it.module);
    }
    shaderStages.clear();
    return true;
}
void vulkan::pipeline::Graphics::Destroy(vk::Device device)noexcept{
    spdlog::debug("in function {}:pipelines:{:#x}", __FUNCTION__, (uintptr_t)(VkPipeline)pipelines);
    device.destroyPipeline(pipelines);
    scissor.clear();
    viewport.clear();
    colorFormat.clear();
    shaderStages.clear();
    dynamicState.clear();
    bindingDescriptions.clear();
    attributeDescriptions.clear();
    for (auto&it:shaderStages){
        device.destroyShaderModule(it.module);
    }
    shaderStages.clear();
}
vulkan::pipeline::Compute::Compute(){
}
vulkan::pipeline::Compute::~Compute(){
}
void vulkan::pipeline::Compute::Bind(vk::CommandBuffer command)const noexcept{
    command.bindPipeline(vk::PipelineBindPoint::eCompute, pipelines);
}
bool vulkan::pipeline::Compute::Create(vk::Device device, vk::PipelineLayout layout, vk::PipelineCache cache){
    if(shaderStages.empty()){
        spdlog::error("in function {}:shaders is empty", __FUNCTION__);
        device.destroyShaderModule(shaderStages[0].module);
        return false;
    }
    vk::ComputePipelineCreateInfo info = {};
    info.sType = vk::StructureType::eComputePipelineCreateInfo;
    info.layout = layout;
    info.stage = shaderStages[0];
    auto result = device.createComputePipeline(cache, info);
    if(result.result == vk::Result::eSuccess){
        pipelines = result.value;
        spdlog::debug("in function {}:pipelines:{:#x}", __FUNCTION__, (uintptr_t)(VkPipeline)pipelines);
    }
    else{
        VK_CHECK_LOG(result.result);
        device.destroyShaderModule(shaderStages[0].module);
        return false;
    }
    device.destroyShaderModule(shaderStages[0].module);
    return true;
}
void vulkan::pipeline::Compute::Destroy(vk::Device device)noexcept{
    spdlog::debug("in function {}:pipelines:{:#x}", __FUNCTION__, (uintptr_t)(VkPipeline)pipelines);
    shaderStages.clear();
    device.destroyPipeline(pipelines);
}
void vulkan::pipeline::Compute::PushShaders(vk::Device device, const std::string &file){
    pipeline::Pipeline::PushShaders(device, vk::ShaderStageFlagBits::eCompute, file);
}
void vulkan::pipeline::Compute::PushShaders(vk::Device device, const uint32_t *code, uint32_t size){
    pipeline::Pipeline::PushShaders(device, vk::ShaderStageFlagBits::eCompute, code, size);
}
vulkan::pipeline::Cache::Cache(){
}
vulkan::pipeline::Cache::~Cache(){
}

void vulkan::pipeline::Cache::Create(vk::Device device, const std::string &file){
    cache = tools::createPipelineCache(device, file);
}

void vulkan::pipeline::Cache::Destroy(vk::Device device, const std::string &file){
    tools::destroyPipelineCache(device, file, cache);
}
