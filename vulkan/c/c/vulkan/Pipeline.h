// #ifndef PIPELINE_INCLUDE_H 
// #define PIPELINE_INCLUDE_H
// #include <stdio.h>
// #include <string.h>
// #include <vulkan/vulkan.h>
// VkShaderModuleCreateInfo shaderModuleCreateInfo(uint32_t size){
// 	VkShaderModuleCreateInfo info{};
// 	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
// 	info.codeSize = size;
// 	return info;
// }
// VkPushConstantRange pushConstantRange(VkShaderStageFlags stageFlags, uint32_t size, uint32_t offset){
// 	VkPushConstantRange pushConstantRange {};
// 	pushConstantRange.stageFlags = stageFlags;
// 	pushConstantRange.offset = offset;
// 	pushConstantRange.size = size;
// 	return pushConstantRange;
// }
// VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(const VkDescriptorSetLayout* pSetLayouts, uint32_t setLayoutCount = 1){
// 	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
// 	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
// 	pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
// 	pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
// 	return pipelineLayoutCreateInfo;
// }

// VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(uint32_t setLayoutCount = 1, uint32_t pushConstantRangeCount = 0){
// 	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
// 	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
// 	pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
// 	pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
// 	return pipelineLayoutCreateInfo;
// }
// VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(){
// 	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
// 	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
// 	return pipelineVertexInputStateCreateInfo;
// }

// VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>&vertexBindingDescriptions, const std::vector<VkVertexInputAttributeDescription>&vertexAttributeDescriptions){
// 	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
// 	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
// 	pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
// 	pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
// 	pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
// 	pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
// 	return pipelineVertexInputStateCreateInfo;
// }

// VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitiveRestartEnable){
// 	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
// 	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
// 	pipelineInputAssemblyStateCreateInfo.topology = topology;
// 	pipelineInputAssemblyStateCreateInfo.flags = flags;
// 	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
// 	return pipelineInputAssemblyStateCreateInfo;
// }
// VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateFlags flags = 0){
// 	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
// 	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
// 	pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
// 	pipelineRasterizationStateCreateInfo.cullMode = cullMode;
// 	pipelineRasterizationStateCreateInfo.frontFace = frontFace;
// 	pipelineRasterizationStateCreateInfo.flags = flags;
// 	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
// 	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
// 	return pipelineRasterizationStateCreateInfo;
// }

// VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(VkColorComponentFlags colorWriteMask, VkBool32 blendEnable){
// 	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState {};
// 	pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
// 	pipelineColorBlendAttachmentState.blendEnable = blendEnable;
// 	return pipelineColorBlendAttachmentState;
// }

// VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState * pAttachments){
// 	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo {};
// 	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
// 	pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
// 	pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
// 	return pipelineColorBlendStateCreateInfo;
// }
// VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp){
// 	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo {};
// 	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
// 	pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
// 	pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
// 	pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
// 	pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
// 	return pipelineDepthStencilStateCreateInfo;
// }

// VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateFlags flags = 0){
// 	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
// 	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
// 	pipelineViewportStateCreateInfo.viewportCount = viewportCount;
// 	pipelineViewportStateCreateInfo.scissorCount = scissorCount;
// 	pipelineViewportStateCreateInfo.flags = flags;
// 	return pipelineViewportStateCreateInfo;
// }

// VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples, VkPipelineMultisampleStateCreateFlags flags = 0){
// 	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo {};
// 	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
// 	pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
// 	pipelineMultisampleStateCreateInfo.flags = flags;
// 	return pipelineMultisampleStateCreateInfo;
// }
// VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(const VkDynamicState * pDynamicStates, uint32_t dynamicStateCount, VkPipelineDynamicStateCreateFlags flags = 0){
// 	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
// 	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
// 	pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
// 	pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
// 	pipelineDynamicStateCreateInfo.flags = flags;
// 	return pipelineDynamicStateCreateInfo;
// }

// VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>& pDynamicStates, VkPipelineDynamicStateCreateFlags flags = 0){
// 	VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
// 	pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
// 	pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
// 	pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
// 	pipelineDynamicStateCreateInfo.flags = flags;
// 	return pipelineDynamicStateCreateInfo;
// }

// VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo(uint32_t patchControlPoints){
// 	VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo {};
// 	pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
// 	pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
// 	return pipelineTessellationStateCreateInfo;
// }

// VkGraphicsPipelineCreateInfo pipelineCreateInfo(VkPipelineLayout layout, VkRenderPass renderPass, VkPipelineCreateFlags flags = 0){
// 	VkGraphicsPipelineCreateInfo pipelineCreateInfo {};
// 	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
// 	pipelineCreateInfo.layout = layout;
// 	pipelineCreateInfo.renderPass = renderPass;
// 	pipelineCreateInfo.flags = flags;
// 	pipelineCreateInfo.basePipelineIndex = -1;
// 	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
// 	return pipelineCreateInfo;
// }
// VkGraphicsPipelineCreateInfo pipelineCreateInfo(){
// 	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
// 	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
// 	pipelineCreateInfo.basePipelineIndex = -1;
// 	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
// 	return pipelineCreateInfo;
// }

// VkComputePipelineCreateInfo computePipelineCreateInfo(VkPipelineLayout layout, VkPipelineCreateFlags flags = 0){
// 	VkComputePipelineCreateInfo computePipelineCreateInfo {};
// 	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
// 	computePipelineCreateInfo.layout = layout;
// 	computePipelineCreateInfo.flags = flags;
// 	return computePipelineCreateInfo;
// }
// VkVertexInputBindingDescription vertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate){
// 	VkVertexInputBindingDescription vInputBindDescription {};
// 	vInputBindDescription.binding = binding;
// 	vInputBindDescription.stride = stride;
// 	vInputBindDescription.inputRate = inputRate;
// 	return vInputBindDescription;
// }
// VkVertexInputAttributeDescription vertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset){
// 	VkVertexInputAttributeDescription vInputAttribDescription {};
// 	vInputAttribDescription.location = location;
// 	vInputAttribDescription.binding = binding;
// 	vInputAttribDescription.format = format;
// 	vInputAttribDescription.offset = offset;
// 	return vInputAttribDescription;
// }
// /** @brief Initialize a map entry for a shader specialization constant */
// VkSpecializationMapEntry specializationMapEntry(uint32_t constantID, uint32_t offset, size_t size){
// 	VkSpecializationMapEntry specializationMapEntry{};
// 	specializationMapEntry.constantID = constantID;
// 	specializationMapEntry.offset = offset;
// 	specializationMapEntry.size = size;
// 	return specializationMapEntry;
// }

// /** @brief Initialize a specialization constant info structure to pass to a shader stage */
// VkSpecializationInfo specializationInfo(uint32_t mapEntryCount, const VkSpecializationMapEntry* mapEntries, size_t dataSize, const void* data){
// 	VkSpecializationInfo specializationInfo{};
// 	specializationInfo.mapEntryCount = mapEntryCount;
// 	specializationInfo.pMapEntries = mapEntries;
// 	specializationInfo.dataSize = dataSize;
// 	specializationInfo.pData = data;
// 	return specializationInfo;
// }

// /** @brief Initialize a specialization constant info structure to pass to a shader stage */
// VkSpecializationInfo specializationInfo(const std::vector<VkSpecializationMapEntry> &mapEntries, size_t dataSize, const void* data){
// 	VkSpecializationInfo specializationInfo{};
// 	specializationInfo.mapEntryCount = static_cast<uint32_t>(mapEntries.size());
// 	specializationInfo.pMapEntries = mapEntries.data();
// 	specializationInfo.dataSize = dataSize;
// 	specializationInfo.pData = data;
// 	return specializationInfo;
// }
// // Ray tracing related
// VkAccelerationStructureGeometryKHR accelerationStructureGeometryKHR(){
// 	VkAccelerationStructureGeometryKHR accelerationStructureGeometryKHR{};
// 	accelerationStructureGeometryKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
// 	return accelerationStructureGeometryKHR;
// }

// VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR(){
// 	VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR{};
// 	accelerationStructureBuildGeometryInfoKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
// 	return accelerationStructureBuildGeometryInfoKHR;
// }

// VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR(){
// 	VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR{};
// 	accelerationStructureBuildSizesInfoKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
// 	return accelerationStructureBuildSizesInfoKHR;
// }

// VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR(){
// 	VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR{};
// 	rayTracingShaderGroupCreateInfoKHR.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
// 	return rayTracingShaderGroupCreateInfoKHR;
// }

// VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR(){
// 	VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR{};
// 	rayTracingPipelineCreateInfoKHR.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
// 	return rayTracingPipelineCreateInfoKHR;
// }
// uint32_t GetFileSize(FILE *fp){
// 	uint32_t size = 0;
// 	if(fp){
// 		fseek(fp, 0, SEEK_END);
// 		size = ftell(fp);
// 		fseek(fp, 0, SEEK_SET);
// 	}
// 	return size;
// }
// uint32_t GetFileContent(const std::string&file, std::vector<uint32_t>&data){
// 	FILE *fp = fopen(file.c_str(), "rb");
// 	if(!fp){
// 		perror("open file error");
// 		printf("file is %s\n", file.c_str());
// 		return 0;
// 	}
// 	uint32_t size = GetFileSize(fp);
// 	data.resize(size / sizeof(uint32_t));
// 	fread(data.data(), size, 1, fp);
// 	fclose(fp);
// 	return size;
// }
// bool WriteFileContent(const std::string&file, const void *data, uint32_t size){
// 	if(data == nullptr){
// 		printf("write file content error:data is nullptr\n");
// 		return false;
// 	}
// 	FILE *fp = fopen(file.c_str(), "wb");
// 	if(!fp){
// 		perror("open file error");
// 		printf("file name is %s\n", file.c_str());
// 		return false;
// 	}
// 	fwrite(data, size, 1, fp);
// 	fclose(fp);
// 	return true; 
// }
// VkViewport Viewport(float width, float height, float minDepth = 0, float maxDepth = 1){
// 	VkViewport viewport {};
// 	viewport.width = width;
// 	viewport.height = height;
// 	viewport.minDepth = minDepth;
// 	viewport.maxDepth = maxDepth;
// 	return viewport;
// }
// VkRect2D Rect2D(int32_t width, int32_t height, int32_t offsetX = 0, int32_t offsetY = 0){
// 	VkRect2D rect2D {};
// 	rect2D.extent.width = width;
// 	rect2D.extent.height = height;
// 	rect2D.offset.x = offsetX;
// 	rect2D.offset.y = offsetY;
// 	return rect2D;
// }
// VkPipelineShaderStageCreateInfo LoadShader(VkDevice device, VkShaderStageFlagBits stage, const uint32_t *code, uint32_t size, const char *name = "main"){
// 	VkShaderModuleCreateInfo shaderModuleCreateInfo = initializers::shaderModuleCreateInfo(size);
// 	shaderModuleCreateInfo.pCode = code;
// 	VkPipelineShaderStageCreateInfo info {};
// 	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	info.pName = name;
// 	info.stage = stage;
// 	vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &info.module);
// 	return info;
// }
// VkPipelineShaderStageCreateInfo LoadShader(VkDevice device, const char *filename, VkShaderStageFlagBits stage, const char *name = "main"){
// 	std::vector<uint32_t>code;
// 	GetFileContent(filename, code);
// 	return LoadShader(device, stage, code.data(), code.size() * sizeof(uint32_t), name);
// }
// VkResult CreatePipelineCache(VkDevice device, const std::string&cacheFile, VkPipelineCache&cache){
// 	std::vector<uint32_t>cacheData;
// 	VkPipelineCacheCreateInfo cacheInfo = {};
// 	cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
// 	if(GetFileContent(cacheFile.c_str(), cacheData)){
// 		cacheInfo.initialDataSize = cacheData.size() * sizeof(uint32_t);
// 		cacheInfo.pInitialData = cacheData.data();
// 	}
// 	return vkCreatePipelineCache(device, &cacheInfo, nullptr, &cache);
// }
// //文件名设置为""则不把管线缓存写到文件
// void DestroyPipelineCache(VkDevice device, const std::string&cacheFile, VkPipelineCache&cache){
// 	if(cacheFile != ""){
// 		size_t cacheDataSize;
// 		std::vector<uint32_t>cacheData;
// 		vkGetPipelineCacheData(device, cache, &cacheDataSize, nullptr);
// 		cacheData.resize(cacheDataSize / sizeof(char));
// 		vkGetPipelineCacheData(device, cache, &cacheDataSize, cacheData.data());
// 		WriteFileContent(cacheFile, cacheData.data(), cacheData.size() * sizeof(uint32_t));
// 	}
// 	if(cache != VK_NULL_HANDLE)vkDestroyPipelineCache(device, cache, nullptr);
// }
// // bool isValidPipelineCacheData(VkPhysicalDevice physicalDevice, const void *buffer, uint32_t size){
// // 	if(size > 32) {
// // 		uint32_t header = 0;
// // 		uint32_t version = 0;
// // 		uint32_t vendor = 0;
// // 		uint32_t deviceID = 0;
// // 		VkPhysicalDeviceProperties deviceProperties;
// // 		uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};
// // 		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

// // 		memcpy(&header, (uint8_t *)buffer + 0, 4);
// // 		memcpy(&version, (uint8_t *)buffer + 4, 4);
// // 		memcpy(&vendor, (uint8_t *)buffer + 8, 4);
// // 		memcpy(&deviceID, (uint8_t *)buffer + 12, 4);
// // 		memcpy(pipelineCacheUUID, (uint8_t *)buffer + 16, VK_UUID_SIZE);

// // 		if (header <= 0) {
// // 			std::cerr << "bad pipeline cache data header length" << std::endl;
// // 			return false;
// // 		}

// // 		if (version != VK_PIPELINE_CACHE_HEADER_VERSION_ONE) {
// // 			std::cerr << "unsupported cache header version" << std::endl;
// // 			std::cerr << "cache contains: 0x" << std::hex << version << std::endl;
// // 		}

// // 		if (vendor != deviceProperties.vendorID) {
// // 			std::cerr << "vendor id mismatch" << std::endl;
// // 			std::cerr << "cache contains: 0x" << std::hex << vendor << std::endl;
// // 			std::cerr << "driver expects: 0x" << deviceProperties.vendorID << std::endl;
// // 			return false;
// // 		}

// // 		if (deviceID != deviceProperties.deviceID) {
// // 			std::cerr << "device id mismatchin" << std::endl;
// // 			std::cerr << "cache contains: 0x" << std::hex << deviceID << std::endl;
// // 			std::cerr << "driver expects: 0x" << deviceProperties.deviceID << std::endl;
// // 			return false;
// // 		}
// // 		if (memcmp(pipelineCacheUUID, deviceProperties.pipelineCacheUUID, sizeof(pipelineCacheUUID)) != 0) {
// // 			std::cerr << "uuid mismatch" << std::endl;
// // 			std::cerr << "cache contains: " << std::endl;

// // 			auto fn = [](uint8_t* uuid) {
// // 				for(int i = 0; i < 16; i++) {
// // 					std::cout << (int)uuid[i] << " ";
// // 					if(i % 4 == 3)
// // 						std::cerr << std::endl;
// // 				}
// // 				std::cerr << std::endl;
// // 			};
// // 			fn(pipelineCacheUUID);
// // 			std::cerr << "driver expects:" << std::endl;
// // 			fn(deviceProperties.pipelineCacheUUID);
// // 			return false;
// // 		}
// // 		return true;
// // 	}
// // 	return false;
// // }
// #endif
