#include <array>
#include "vulkan.hpp"
// backends/vulkan/glsl_shader.vert, compiled with:
// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
/*
#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;
    gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
}
*/
static uint32_t __glsl_shader_vert_spv[] ={
    0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
};

// glsl_shader.frag, compiled with:
// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
/*
#version 450 core
layout(location = 0) out vec4 fColor;
layout(set=0, binding=0) uniform sampler2D sTexture;
layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
void main()
{
    fColor = In.Color * texture(sTexture, In.UV.st);
}
*/
static uint32_t __glsl_shader_frag_spv[] ={
    0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
};
namespace vulkan{
    void ImGui::CreatePipelineLayout(vk::Device device){
        vk::PipelineLayoutCreateInfo info = vulkan::pipeline::initializers::pipelineLayoutCreateInfo(1, 1);
        vk::PushConstantRange pushConstantRange = vulkan::pipeline::initializers::pushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(glm::vec2) * 2, 0);
        info.pSetLayouts = &mSetLayout;
        info.pPushConstantRanges = &pushConstantRange;
        pipelines.layout = device.createPipelineLayout(info);
    }
    void ImGui::SetupDescriptorSetLayout(vk::Device device){
        std::array bindings = {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
        };
        mSetLayout = device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings));
    }
    void ImGui::CreateFontsTexture(const vulkan::Device&device, vk::Queue graphics, const vulkan::Pool&pool){
        ImGuiIO& io = ::ImGui::GetIO();

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        
        vk::Extent3D size;
        size.depth = 1;
        size.width = width;
        size.height = height;
        mFont.Create(device, size, vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled, vk::Format::eR8G8B8A8Unorm);
        mFont.CreateView(device);
        mFont.Copy(device, pixels, 1, graphics, pool);

        io.Fonts->ClearTexData();
    }
    void ImGui::SetupRenderState(vk::CommandBuffer command, ImDrawData *draw_data, int fb_width, int fb_height){
        command.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.pipeline);
        // Bind Vertex And Index Buffer:
        mRect.Bind(command);

        // Setup viewport:
        vk::Viewport viewport = {};
        viewport.maxDepth = 1;
        viewport.width = fb_width;
        viewport.height = fb_height;
        command.setViewport(0, viewport);

        // Setup scale and translation:
        // Our visible imgui space lies from draw_data->DisplayPps (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        float scale[2];
        scale[0] = 2.0f / draw_data->DisplaySize.x;
        scale[1] = 2.0f / draw_data->DisplaySize.y;
        float translate[2];
        translate[0] = -1.0f - draw_data->DisplayPos.x * scale[0];
        translate[1] = -1.0f - draw_data->DisplayPos.y * scale[1];
        command.pushConstants(pipelines.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::vec2), scale);
        command.pushConstants(pipelines.layout, vk::ShaderStageFlagBits::eVertex, sizeof(glm::vec2), sizeof(glm::vec2), translate);
    }
    static uint32_t ImGui_ImplVulkan_MemoryType(vk::PhysicalDevice physicalDevice, vk::MemoryPropertyFlags properties, uint32_t type_bits){
        vk::PhysicalDeviceMemoryProperties prop = physicalDevice.getMemoryProperties();
        for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
            if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
                return i;
        return 0xFFFFFFFF; // Unable to find memoryType
    }
    ImGui::ImGui(/* args */){
    }

    ImGui::~ImGui(){
    }

    void ImGui::Cleanup(){
        mRect.Destroy(*mDevice);
        mFont.Destroy(*mDevice);
        vk::Device dev = *mDevice;
        dev.destroySampler(mFontSampler);
        dev.destroyPipeline(pipelines.pipeline);
        dev.destroyPipelineLayout(pipelines.layout);
        dev.destroyDescriptorSetLayout(mSetLayout);
    }

    void ImGui::Setup(const vulkan::Device&device, const vulkan::Pool&pool){
        mDevice = &device;
        mBufferMemoryAlignment = 256;
        mFontSampler = vulkan::framework::CreateTextureSampler(device);
        SetupDescriptorSetLayout(device);
        mSet = pool.AllocateDescriptorSets(device, {mSetLayout})[0];
    }
    
    void ImGui::CreatePipeline(vk::Format color, vk::Format depth, vk::RenderPass renderPass, vk::PipelineCache cache){
        CreatePipelineLayout(*mDevice);

        pipelines.pipeline.PushColorBlendAttachmentState();

        pipelines.pipeline.SetRenderPass(renderPass);

        pipelines.pipeline.EnableColorBlend();
        pipelines.pipeline.SetDepthFormat(depth);
        pipelines.pipeline.PushColorFormat(color);
        pipelines.pipeline.PushDynamicState(vk::DynamicState::eScissor);
        pipelines.pipeline.PushDynamicState(vk::DynamicState::eViewport);

        pipelines.pipeline.SetCullMode(vk::CullModeFlagBits::eNone);

        pipelines.pipeline.PushShaders(*mDevice, vk::ShaderStageFlagBits::eVertex, __glsl_shader_vert_spv, sizeof(__glsl_shader_vert_spv));
        pipelines.pipeline.PushShaders(*mDevice, vk::ShaderStageFlagBits::eFragment, __glsl_shader_frag_spv, sizeof(__glsl_shader_frag_spv));

        pipelines.pipeline.SetColorBlendOp(vk::BlendOp::eAdd);
        pipelines.pipeline.SetAlphaBlendOp(vk::BlendOp::eAdd);
        pipelines.pipeline.SetSrcAlphaBlendFactor(vk::BlendFactor::eOne);
        pipelines.pipeline.SetSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
        pipelines.pipeline.SetDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
        pipelines.pipeline.SetDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);

        pipelines.pipeline.PushVertexInputBindingDescription(0, sizeof(ImDrawVert));

        pipelines.pipeline.PushVertInputAttributeDescription(0, vk::Format::eR32G32Sfloat, IM_OFFSETOF(ImDrawVert, pos));
        pipelines.pipeline.PushVertInputAttributeDescription(1, vk::Format::eR32G32Sfloat, IM_OFFSETOF(ImDrawVert, uv));
        pipelines.pipeline.PushVertInputAttributeDescription(2, vk::Format::eR8G8B8A8Unorm, IM_OFFSETOF(ImDrawVert, col));

        pipelines.pipeline.Create(*mDevice, pipelines.layout, cache);
    }
    void ImGui::CreateFont(const char *font, vk::Queue graphics, const vulkan::Pool&pool){
        if(mFont.GetImage() != VK_NULL_HANDLE){
            mFont.Destroy(*mDevice);
        }

        ImGuiIO&io = ::ImGui::GetIO();
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(font, 20, NULL, io.Fonts->GetGlyphRangesChineseFull());

        CreateFontsTexture(*mDevice, graphics, pool);

        std::vector bindings = {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
        };
        vulkan::framework::UpdateDescriptorSets(*mDevice, bindings, {}, { mFont }, mSet, mFontSampler);

        // Store our identifier
        io.Fonts->SetTexID((ImTextureID)(VkDescriptorSet)mSet);
    }

    void ImGui::RenderDrawData(vk::CommandBuffer command, ImDrawData *draw_data){
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        if (draw_data->TotalVtxCount > 0){
            // Create or resize the vertex/index buffers
            size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
            size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
            if(!mRect.IsVaildIndex() || index_size != mRect.GetIndexSize())
                mRect.CreateIndexBuffer(*mDevice, index_size, true, sizeof(ImDrawIdx) == 2 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);
            if(!mRect.IsVaildVertex() || vertex_size != mRect.GetVertexSize())
                mRect.CreateVertexBuffer(*mDevice, vertex_size, 0, true);
            // Upload vertex/index data into a single contiguous GPU buffer
            ImDrawIdx* idx_dst = (ImDrawIdx*)mRect.MapMemoryIndex(*mDevice);
            ImDrawVert* vtx_dst = (ImDrawVert*)mRect.MapMemoryVertex(*mDevice);
            for (int n = 0; n < draw_data->CmdListsCount; n++){
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
                vtx_dst += cmd_list->VtxBuffer.Size;
                idx_dst += cmd_list->IdxBuffer.Size;
            }
            mRect.FlushMappedMemoryRanges(*mDevice);
            mRect.UnMapMemoryIndex(*mDevice);
            mRect.UnMapMemoryVertex(*mDevice);
        }

        // Setup desired Vulkan state
        SetupRenderState(command, draw_data, fb_width, fb_height);

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        // Render command lists
        // (Because we merged all buffers into a single one, we maintain our own offset into them)
        int global_vtx_offset = 0;
        int global_idx_offset = 0;
        for (int n = 0; n < draw_data->CmdListsCount; n++){
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++){
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL){
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        SetupRenderState(command, draw_data, fb_width, fb_height);
                    else
                        pcmd->UserCallback(cmd_list, pcmd);
                }
                else{
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                    ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);

                    // Clamp to viewport as vkCmdSetScissor() won't accept values that are off bounds
                    if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
                    if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
                    if (clip_max.x > fb_width) { clip_max.x = (float)fb_width; }
                    if (clip_max.y > fb_height) { clip_max.y = (float)fb_height; }
                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                        continue;

                    // Apply scissor/clipping rectangle
                    vk::Rect2D scissor;
                    scissor.offset.x = (int32_t)(clip_min.x);
                    scissor.offset.y = (int32_t)(clip_min.y);
                    scissor.extent.width = (uint32_t)(clip_max.x - clip_min.x);
                    scissor.extent.height = (uint32_t)(clip_max.y - clip_min.y);
                    command.setScissor(0, 1, &scissor);

                    // Bind DescriptorSet with font or user texture
                    VkDescriptorSet desc_set[1] = { (VkDescriptorSet)pcmd->TextureId };
                    if (sizeof(ImTextureID) < sizeof(ImU64)){
                        // We don't support texture switches if ImTextureID hasn't been redefined to be 64-bit. Do a flaky check that other textures haven't been used.
                        IM_ASSERT(pcmd->TextureId == (ImTextureID)(VkDescriptorSet)mSet);
                        desc_set[0] = mSet;
                    }
                    /*
                        使用推送描述符集则不需要每次都创建一个描述符集。
                        只需要图片的view
                    */
                    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.layout, 0, 1, desc_set, 0, NULL);

                    // Draw
                    vkCmdDrawIndexed(command, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
                }
            }
            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }

        // Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
        // Our last values will leak into user/application rendering IF:
        // - Your app uses a pipeline with VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR dynamic state
        // - And you forgot to call vkCmdSetViewport() and vkCmdSetScissor() yourself to explicitely set that state.
        // If you use VK_DYNAMIC_STATE_VIEWPORT or VK_DYNAMIC_STATE_SCISSOR you are responsible for setting the values before rendering.
        // In theory we should aim to backup/restore those values but I am not sure this is possible.
        // We perform a call to vkCmdSetScissor() to set back a full viewport which is likely to fix things for 99% users but technically this is not perfect. (See github #4644)
        VkRect2D scissor = { { 0, 0 }, { (uint32_t)fb_width, (uint32_t)fb_height } };
        vkCmdSetScissor(command, 0, 1, &scissor);
    }
}