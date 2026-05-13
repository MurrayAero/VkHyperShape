#ifndef PIPELINE_INCLUDE_HPP
#define PIPELINE_INCLUDE_HPP
#include <string>
#include <vector>
#include "image.hpp"
namespace vulkan{
	namespace pipeline{
		namespace initializers{
			inline auto scissor(uint32_t width, uint32_t height, int32_t offsetX = 0, int32_t offsetY = 0){
				vk::Rect2D scissor {};
				scissor.offset.x = offsetX;
				scissor.offset.y = offsetY;
				scissor.extent.width = width;
				scissor.extent.height = height;
				return scissor;
			}
			inline vk::Viewport viewport(uint32_t width, uint32_t height, int32_t offsetX = 0, int32_t offsetY = 0, float minDepth = 0.0f, float maxDepth = 1.0f){
				vk::Viewport viewport {};
				viewport.width = static_cast<float>(width);
				viewport.height = static_cast<float>(height);
				viewport.minDepth = minDepth;
				viewport.maxDepth = maxDepth;
				viewport.x = offsetX;
				viewport.y = offsetY;
				return viewport;
			}
			inline vk::ShaderModuleCreateInfo shaderModuleCreateInfo(uint32_t size){
				vk::ShaderModuleCreateInfo info{};
				info.sType = vk::StructureType::eShaderModuleCreateInfo;
				info.codeSize = size;
				return info;
			}
			inline vk::PushConstantRange pushConstantRange(vk::ShaderStageFlags stageFlags, uint32_t size, uint32_t offset = 0){
				vk::PushConstantRange pushConstantRange {};
				pushConstantRange.stageFlags = stageFlags;
				pushConstantRange.offset = offset;
				pushConstantRange.size = size;
				return pushConstantRange;
			}
			inline vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(const vk::DescriptorSetLayout* pSetLayouts, uint32_t setLayoutCount = 1){
				vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
				pipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
				pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
				pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
				return pipelineLayoutCreateInfo;
			}

			inline vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(uint32_t setLayoutCount = 1, uint32_t pushConstantRangeCount = 0){
				vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
				pipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
				pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
				pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRangeCount;
				return pipelineLayoutCreateInfo;
			}
			inline vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(){
				vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo {};
				pipelineVertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
				return pipelineVertexInputStateCreateInfo;
			}

			inline vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(const std::vector<vk::VertexInputBindingDescription>&vertexBindingDescriptions, const std::vector<vk::VertexInputAttributeDescription>&vertexAttributeDescriptions){
				vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
				pipelineVertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
				pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
				pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
				pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
				pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
				return pipelineVertexInputStateCreateInfo;
			}

			inline vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology topology, vk::Bool32 primitiveRestartEnable, vk::PipelineInputAssemblyStateCreateFlags flags = {}){
				vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo {};
				pipelineInputAssemblyStateCreateInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
				pipelineInputAssemblyStateCreateInfo.topology = topology;
				pipelineInputAssemblyStateCreateInfo.flags = flags;
				pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
				return pipelineInputAssemblyStateCreateInfo;
			}
			inline vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace, vk::PipelineRasterizationStateCreateFlags flags = {}){
				vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
				pipelineRasterizationStateCreateInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
				pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
				pipelineRasterizationStateCreateInfo.cullMode = cullMode;
				pipelineRasterizationStateCreateInfo.frontFace = frontFace;
				pipelineRasterizationStateCreateInfo.flags = flags;
				pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
				pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
				return pipelineRasterizationStateCreateInfo;
			}

			inline vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(vk::ColorComponentFlags colorWriteMask, vk::Bool32 blendEnable){
				vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState {};
				pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
				pipelineColorBlendAttachmentState.blendEnable = blendEnable;
				return pipelineColorBlendAttachmentState;
			}

			inline vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(uint32_t attachmentCount, const vk::PipelineColorBlendAttachmentState * pAttachments){
				vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo {};
				pipelineColorBlendStateCreateInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
				pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
				pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
				return pipelineColorBlendStateCreateInfo;
			}
			inline vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(vk::Bool32 depthTestEnable, vk::Bool32 depthWriteEnable, vk::CompareOp depthCompareOp){
				vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo {};
				pipelineDepthStencilStateCreateInfo.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
				pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
				pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
				pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
				pipelineDepthStencilStateCreateInfo.back.compareOp = vk::CompareOp::eAlways;
				return pipelineDepthStencilStateCreateInfo;
			}

			inline vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(uint32_t viewportCount, uint32_t scissorCount, vk::PipelineViewportStateCreateFlags flags = {}){
				vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
				pipelineViewportStateCreateInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
				pipelineViewportStateCreateInfo.viewportCount = viewportCount;
				pipelineViewportStateCreateInfo.scissorCount = scissorCount;
				pipelineViewportStateCreateInfo.flags = flags;
				return pipelineViewportStateCreateInfo;
			}

			inline vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits rasterizationSamples, vk::PipelineMultisampleStateCreateFlags flags = {}){
				vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo {};
				pipelineMultisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
				pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
				pipelineMultisampleStateCreateInfo.flags = flags;
				return pipelineMultisampleStateCreateInfo;
			}
			inline vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(const vk::DynamicState *pDynamicStates, uint32_t dynamicStateCount, vk::PipelineDynamicStateCreateFlags flags = {}){
				vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
				pipelineDynamicStateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
				pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
				pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
				pipelineDynamicStateCreateInfo.flags = flags;
				return pipelineDynamicStateCreateInfo;
			}

			inline vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(const std::vector<vk::DynamicState>& pDynamicStates, vk::PipelineDynamicStateCreateFlags flags = {}){
				vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
				pipelineDynamicStateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
				pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
				pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
				pipelineDynamicStateCreateInfo.flags = flags;
				return pipelineDynamicStateCreateInfo;
			}

			inline vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo(uint32_t patchControlPoints){
				vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo {};
				pipelineTessellationStateCreateInfo.sType = vk::StructureType::ePipelineTessellationStateCreateInfo;
				pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
				return pipelineTessellationStateCreateInfo;
			}

			inline vk::GraphicsPipelineCreateInfo pipelineCreateInfo(vk::PipelineLayout layout, vk::RenderPass renderPass, vk::PipelineCreateFlags flags = {}){
				vk::GraphicsPipelineCreateInfo pipelineCreateInfo {};
				pipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
				pipelineCreateInfo.layout = layout;
				pipelineCreateInfo.renderPass = renderPass;
				pipelineCreateInfo.flags = flags;
				pipelineCreateInfo.basePipelineIndex = -1;
				pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
				return pipelineCreateInfo;
			}
			inline vk::GraphicsPipelineCreateInfo pipelineCreateInfo(){
				vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
				pipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
				pipelineCreateInfo.basePipelineIndex = -1;
				pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
				return pipelineCreateInfo;
			}

			inline vk::ComputePipelineCreateInfo computePipelineCreateInfo(vk::PipelineLayout layout, vk::PipelineCreateFlags flags = {}){
				vk::ComputePipelineCreateInfo computePipelineCreateInfo {};
				computePipelineCreateInfo.sType = vk::StructureType::eComputePipelineCreateInfo;
				computePipelineCreateInfo.layout = layout;
				computePipelineCreateInfo.flags = flags;
				return computePipelineCreateInfo;
			}
			inline vk::VertexInputBindingDescription vertexInputBindingDescription(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate){
				vk::VertexInputBindingDescription vInputBindDescription {};
				vInputBindDescription.binding = binding;
				vInputBindDescription.stride = stride;
				vInputBindDescription.inputRate = inputRate;
				return vInputBindDescription;
			}
			inline vk::VertexInputAttributeDescription vertexInputAttributeDescription(uint32_t binding, uint32_t location, vk::Format format, uint32_t offset){
				vk::VertexInputAttributeDescription vInputAttribDescription {};
				vInputAttribDescription.location = location;
				vInputAttribDescription.binding = binding;
				vInputAttribDescription.format = format;
				vInputAttribDescription.offset = offset;
				return vInputAttribDescription;
			}
			/** @brief Initialize a map entry for a shader specialization constant */
			inline vk::SpecializationMapEntry specializationMapEntry(uint32_t constantID, uint32_t offset, size_t size){
				vk::SpecializationMapEntry specializationMapEntry{};
				specializationMapEntry.constantID = constantID;
				specializationMapEntry.offset = offset;
				specializationMapEntry.size = size;
				return specializationMapEntry;
			}

			/** @brief Initialize a specialization constant info structure to pass to a shader stage */
			inline vk::SpecializationInfo specializationInfo(uint32_t mapEntryCount, const vk::SpecializationMapEntry* mapEntries, size_t dataSize, const void* data){
				vk::SpecializationInfo specializationInfo{};
				specializationInfo.mapEntryCount = mapEntryCount;
				specializationInfo.pMapEntries = mapEntries;
				specializationInfo.dataSize = dataSize;
				specializationInfo.pData = data;
				return specializationInfo;
			}

			/** @brief Initialize a specialization constant info structure to pass to a shader stage */
			inline vk::SpecializationInfo specializationInfo(const std::vector<vk::SpecializationMapEntry> &mapEntries, size_t dataSize, const void* data){
				vk::SpecializationInfo specializationInfo{};
				specializationInfo.mapEntryCount = static_cast<uint32_t>(mapEntries.size());
				specializationInfo.pMapEntries = mapEntries.data();
				specializationInfo.dataSize = dataSize;
				specializationInfo.pData = data;
				return specializationInfo;
			}
			// Ray tracing related
			inline vk::AccelerationStructureGeometryKHR accelerationStructureGeometryKHR(){
				vk::AccelerationStructureGeometryKHR accelerationStructureGeometryKHR{};
				accelerationStructureGeometryKHR.sType = vk::StructureType::eAccelerationStructureGeometryKHR;
				return accelerationStructureGeometryKHR;
			}

			inline vk::AccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR(){
				vk::AccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR{};
				accelerationStructureBuildGeometryInfoKHR.sType = vk::StructureType::eAccelerationStructureBuildGeometryInfoKHR;
				return accelerationStructureBuildGeometryInfoKHR;
			}

			inline vk::AccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR(){
				vk::AccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR{};
				accelerationStructureBuildSizesInfoKHR.sType = vk::StructureType::eAccelerationStructureBuildSizesInfoKHR;
				return accelerationStructureBuildSizesInfoKHR;
			}

			inline vk::RayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR(){
				vk::RayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR{};
				rayTracingShaderGroupCreateInfoKHR.sType = vk::StructureType::eRayTracingShaderGroupCreateInfoKHR;
				return rayTracingShaderGroupCreateInfoKHR;
			}

			inline vk::RayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR(){
				vk::RayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR{};
				rayTracingPipelineCreateInfoKHR.sType = vk::StructureType::eRayTracingPipelineCreateInfoKHR;
				return rayTracingPipelineCreateInfoKHR;
			}
		}
		namespace tools{
			vk::PipelineCache createPipelineCache(vk::Device device, const std::string &file);
			//文件名为空则不把管线缓存写到文件
			void destroyPipelineCache(vk::Device device, const std::string &file, vk::PipelineCache cache);
			
			uint32_t GetFileSize(FILE *fp);
			
			template<typename DATATYPE>
			uint32_t GetFileContent(const std::string&file, std::vector<DATATYPE>&data){
				FILE *fp = fopen(file.c_str(), "rb");
				if(!fp){
					perror("open file error");
					printf("file is %s\n", file.c_str());
					return 0;
				}
				uint32_t size = GetFileSize(fp);
				data.resize(size / sizeof(DATATYPE));
				fread(data.data(), size, 1, fp);
				fclose(fp);
				return size;
			}

			vk::PipelineShaderStageCreateInfo loadShader(vk::Device device, const char *filename, vk::ShaderStageFlagBits stage);
			vk::PipelineShaderStageCreateInfo loadShader(vk::Device device, vk::ShaderStageFlagBits stage, const uint32_t *code, uint32_t size);

			bool WriteFileContent(const std::string &file, const void *data, uint32_t size);
		}
	}
}
namespace vulkan{
	namespace pipeline{
		class Cache{
			vk::PipelineCache cache;
		public:
			Cache();
			// Cache(const Cache&) = delete;
			~Cache();
			void Create(vk::Device device, const std::string&file);
			//文件名为空则不把管线缓存写到文件
			void Destroy(vk::Device device, const std::string&file);

			inline operator vk::PipelineCache() const noexcept{
				return cache;
			}
		};
	}
}
namespace vulkan{
	namespace pipeline{
		class Pipeline{
		protected:
			vk::Pipeline pipelines;
			std::vector<vk::PipelineShaderStageCreateInfo>shaderStages;
		public:
			Pipeline();
			~Pipeline();

			virtual void Bind(vk::CommandBuffer command) const noexcept = 0;
			virtual void Create(vk::Device device, vk::PipelineLayout layout, vk::PipelineCache cache) = 0;

			virtual void Destroy(vk::Device device)noexcept = 0;

			virtual void PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const std::string&shaders);
			virtual void PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const uint32_t *code, uint32_t size);
			//暂时只支持一个pSpecializationMapEntry
			void PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const std::string&shaders, const vk::SpecializationMapEntry *pSpecializationMapEntry, const void *pData);
			void PushShaders(vk::Device device, vk::ShaderStageFlagBits stage, const uint32_t *code, uint32_t size, const vk::SpecializationMapEntry *pSpecializationMapEntry, const void *pData);
			
			inline bool operator==(const Pipeline&pipeline)const noexcept{
				return pipelines == pipeline.GetPipeline();
			}
			inline bool operator!=(const Pipeline&pipeline)const noexcept{
				return pipelines != pipeline.GetPipeline();
			}
			inline operator vk::Pipeline() const noexcept{
				return pipelines;
			}
			inline vk::Pipeline GetPipeline()const noexcept{
				return pipelines;
			}
		};
	}
}
namespace vulkan{
	namespace pipeline{
	    class Graphics:public Pipeline{
			std::vector<vk::Rect2D>scissor = {};
			std::vector<vk::Viewport>viewport = {};
			vk::Format depthFormat = vk::Format::eUndefined;
			std::vector<vk::Format>colorFormat;
			std::vector<vk::DynamicState>dynamicState;
			vk::RenderPass renderPass = VK_NULL_HANDLE;
			std::vector<vk::VertexInputBindingDescription>bindingDescriptions;
			std::vector<vk::VertexInputAttributeDescription>attributeDescriptions;
			std::vector<vk::PipelineColorBlendAttachmentState>blendAttachmentState;
    		vk::PipelineTessellationStateCreateInfo tessellationState = initializers::pipelineTessellationStateCreateInfo(4);
			vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR| vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
			vk::PipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(vk::SampleCountFlagBits::e1);
			vk::PipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(vk::False, vk::True, vk::CompareOp::eLessOrEqual);
			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(vk::PrimitiveTopology::eTriangleList, vk::False);
			vk::PipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise);
			inline bool isWindowSizeInvalid(uint32_t index = 0)const noexcept{
				return viewport.empty() || scissor.empty() || viewport[index].width == 0 || viewport[index].height == 0 || scissor[index].extent.width == 0 || scissor[index].extent.height == 0;
			}
		public:
			Graphics();
			~Graphics();
			inline auto operator=(const Graphics&pipeline)noexcept{
				memcpy(this, &pipeline, sizeof(Graphics));
				return *this;
			}
			virtual void Bind(vk::CommandBuffer command)const noexcept;
			virtual void Create(vk::Device device, vk::PipelineLayout layout, vk::PipelineCache cache);
			virtual void Destroy(vk::Device device)noexcept;

			inline void PushDynamicState(vk::DynamicState state){
				dynamicState.push_back(state);
			}
			inline void SetRenderPass(vk::RenderPass renderPass)noexcept{
				this->renderPass = renderPass;
			}
			//不使用动态渲染不能调用该函数
			inline void SetDepthFormat(vk::Format format)noexcept{
				depthFormat = format;
			}
			inline void EnableColorBlend(uint32_t index = 0)noexcept{
				blendAttachmentState[index].setBlendEnable(true);
			}
			inline void SetColorWriteMask(vk::ColorComponentFlags mask, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setColorWriteMask(mask);
			}
			inline void SetAlphaBlendOp(vk::BlendOp blendOp, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setAlphaBlendOp(blendOp);
			}
			inline void SetColorBlendOp(vk::BlendOp blendOp, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setColorBlendOp(blendOp);
			}
			inline void SetSrcColorBlendFactor(vk::BlendFactor blendFactor, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setSrcColorBlendFactor(blendFactor);
			}
			inline void SetDstColorBlendFactor(vk::BlendFactor blendFactor, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setDstColorBlendFactor(blendFactor);
			}
			inline void SetSrcAlphaBlendFactor(vk::BlendFactor blendFactor, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setSrcAlphaBlendFactor(blendFactor);
			}
			inline void SetDstAlphaBlendFactor(vk::BlendFactor blendFactor, uint32_t index = 0)noexcept{
				blendAttachmentState[index].setDstAlphaBlendFactor(blendFactor);
			}
			//推送一个默认的颜色混合附件
			inline void PushColorBlendAttachmentState()noexcept{
				blendAttachmentState.push_back(vulkan::pipeline::initializers::pipelineColorBlendAttachmentState(colorWriteMask, vk::False));
			}
			inline void PushColorBlendAttachmentState(const vk::PipelineColorBlendAttachmentState&blendAttachmentState)noexcept{
				this->blendAttachmentState.push_back(blendAttachmentState);
			}
			//深度
			inline void EnableDepthTest()noexcept{
				depthStencilState.setDepthTestEnable(true);
			}
			inline void SetDepthWriteEnable(bool depthWriteEnable)noexcept{
				depthStencilState.setDepthWriteEnable(depthWriteEnable);
			}
			inline void SetDepthCompareOp(vk::CompareOp depthCompareOp)noexcept{
				depthStencilState.setDepthCompareOp(depthCompareOp);
			}
			//图元
			inline void SetTopology(vk::PrimitiveTopology topology)noexcept{
				inputAssemblyState.setTopology(topology);
			}
			//光栅化
			inline void SetLineWidth(float lineWidth)noexcept{
				rasterizationState.setLineWidth(lineWidth);
			}
			inline void SetPolygonMode(vk::PolygonMode polygonMode)noexcept{
				rasterizationState.setPolygonMode(polygonMode);
			}
			inline void SetCullMode(vk::CullModeFlags cullMode)noexcept{
				rasterizationState.setCullMode(cullMode);
			}
			inline void SetFrontFace(vk::FrontFace frontFace)noexcept{
				rasterizationState.setFrontFace(frontFace);
			}
			inline void EnableDepthBias()noexcept{
				rasterizationState.setDepthBiasEnable(true);
			}
			inline void SetDepthBiasClamp(float depthBiasClamp)noexcept{
				rasterizationState.setDepthBiasClamp(depthBiasClamp);
			}
			inline void SetDepthBiasConstantFactor(float depthBiasConstantFactor)noexcept{
				rasterizationState.setDepthBiasConstantFactor(depthBiasConstantFactor);
			}
			inline void SetDepthBiasSlopeFactor(float depthBiasSlopeFactor)noexcept{
				rasterizationState.setDepthBiasSlopeFactor(depthBiasSlopeFactor);
			}
			//不使用动态渲染不能调用该函数
			inline void PushColorFormat(vk::Format format){
				colorFormat.push_back(format);
			}
			inline void PushVertexInputBindingDescription(uint32_t binding, uint32_t stride, vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex){
				bindingDescriptions.push_back(vk::VertexInputBindingDescription({ binding, stride, inputRate }));
			}
			inline void SetPatchControlPoints(uint32_t patchControlPoints)noexcept{
				tessellationState.setPatchControlPoints(patchControlPoints);
			}
			inline void PushVertInputAttributeDescription(uint32_t location, vk::Format format, vk::DeviceSize offset, uint32_t binding = 0){
				vk::VertexInputAttributeDescription vertex;
				vertex.offset = offset;
				vertex.format = format;
				vertex.binding = binding;
				vertex.location = location;
				attributeDescriptions.push_back(vertex);
			}

			inline void PushScissor(const vk::Rect2D&scissor)noexcept{
				this->scissor.push_back(scissor);
			}
			inline void PushViewport(const vk::Viewport&viewport)noexcept{
				this->viewport.push_back(viewport);
			}
			inline void PushVertexInputBindingDescription(const vk::VertexInputBindingDescription&binding){
				bindingDescriptions.push_back(binding);
			}
			inline void PushVertInputAttributeDescription(const vk::VertexInputAttributeDescription&attribute){
				attributeDescriptions.push_back(attribute);
			}
			inline void SetMultisampleState(const vk::PipelineMultisampleStateCreateInfo&multisampleState)noexcept{
				this->multisampleState = multisampleState;
			}
			inline void SetDepthStencilState(const vk::PipelineDepthStencilStateCreateInfo&depthStencilState)noexcept{
				this->depthStencilState = depthStencilState;
			}
			inline void SetInputAssemblyState(const vk::PipelineInputAssemblyStateCreateInfo&inputAssemblyState)noexcept{
				this->inputAssemblyState = inputAssemblyState;
			}
			inline void SetRasterizationState(const vk::PipelineRasterizationStateCreateInfo&rasterizationState)noexcept{
				this->rasterizationState = rasterizationState;
			}
		};
	}
}
namespace vulkan{
	namespace pipeline{
		class Compute:public Pipeline{
		public:
			Compute();
			~Compute();
			virtual void Bind(vk::CommandBuffer command)const noexcept;
			virtual void Create(vk::Device device, vk::PipelineLayout layout, vk::PipelineCache cache);
			virtual void Destroy(vk::Device device)noexcept;

			virtual void PushShaders(vk::Device device, const std::string&shaders);
			virtual void PushShaders(vk::Device device, const uint32_t *code, uint32_t size);
		};
	}
}
#endif
