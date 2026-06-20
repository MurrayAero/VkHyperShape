// #include "VulkanDemo.h"
// void CreateTextureImage(VulkanDevice device, const char *name, VkQueue graphics, VulkanPool pool, VulkanCubeImage&image){
//     int nrComponents;
//     std::array<void *, 6>datas;
//     for (size_t i = 0; i < datas.size(); ++i){
//         datas[i] = stbi_load(name, (int *)&image.size.width, (int *)&image.size.height, &nrComponents, STBI_rgb_alpha);
//         if(datas[i] == nullptr){
//             printf("load picture error:%s\n", name);
//             return;
//         }               
//     }
//     image.CreateImage(device, datas.data(), image.size.width, graphics, pool);
//     for (size_t i = 0; i < datas.size(); ++i){
//         stbi_image_free(datas[i]);
//     }
// }
// void VulkanDemo::CreateCube(VulkanDevice device, VkQueue graphics, VulkanPool pool){
//     const glm::vec3 color = glm::vec3(.5);
//     const Vertex vertices[] = {
//         //正面
//         Vertex(glm::vec3(-1.0f, -1.0f,  1.0f), color),//左下角//0
//         Vertex(glm::vec3(1.0f, -1.0f,  1.0f), color),//右下角//1
//         Vertex(glm::vec3(1.0f,  1.0f,  1.0f), color),//右上角//2
//         Vertex(glm::vec3(-1.0f,  1.0f,  1.0f), color),//左上角//3
//         //背面
//         Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), color),//左下角//4
//         Vertex(glm::vec3(1.0f, -1.0f, -1.0f), color),//右下角//5
//         Vertex(glm::vec3(1.0f,  1.0f, -1.0f), color),//右上角//6
//         Vertex(glm::vec3(-1.0f,  1.0f, -1.0f), color),//左上角//7
//     };
//     const uint16_t indices[] = {
//         0,1,2, 2,3,0,//正面
//         1,5,6, 6,2,1, //右面
//         7,6,5, 5,4,7, //背面
//         4,0,3, 3,7,4, 
//         4,5,1, 1,0,4, 
//         3,2,6, 6,7,3,
//     };
//     mCube.CreateIndexBuffer(device, indices, sizeof(indices), graphics, pool);
//     mCube.CreateVertexBuffer(device, vertices, sizeof(vertices), 8, graphics, pool);
// }
// VulkanDemo::VulkanDemo(/* args */){
// }

// VulkanDemo::~VulkanDemo(){
// }

// void VulkanDemo::Cleanup(VkDevice device){
//     vkDestroySampler(device, mTextureSampler, nullptr);

//     mTexture.Destroy(device);

//     mCube.Destroy(device);
//     // mSphere.Destroy(device);
// }

// void VulkanDemo::Setup(VulkanDevice device, VkDescriptorSetLayout layout, VkQueue graphics, VulkanPool pool){
//     vulkanFrame::CreateTextureSampler(device.device, mTextureSampler);

//     CreateCube(device, graphics, pool);
//     // mSphere.CreateSphere(device, 1, glm::vec3(.1), graphics, pool);

//     CreateTextureImage(device, "1.jpg", graphics, pool, mTexture);

//     pool.AllocateDescriptorSets(device.device, &layout, 1, &mSet);
// }

// void VulkanDemo::Draw(VkCommandBuffer command, VkPipelineLayout layout){
//     mCube.Bind(command);
//     vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &mSet, 0, VK_NULL_HANDLE);
//     // mSphere.Draw(command);
//     mCube.Draw(command);
// }

// void VulkanDemo::UpdateSet(VkDevice device, VulkanBuffer camera){
//     VkDescriptorSetLayoutBinding setlayoutBindings[2] = {};
//     // setlayoutBinding.binding = 0;
//     setlayoutBindings[0].descriptorCount = 1;
//     setlayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//     setlayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     setlayoutBindings[1].binding = 1;
//     setlayoutBindings[1].descriptorCount = 1;
//     setlayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//     setlayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//     vulkanFrame::UpdateDescriptorSets(device, setlayoutBindings, sizeof(setlayoutBindings) / sizeof(VkDescriptorSetLayoutBinding), { camera }, { mTexture }, mSet, mTextureSampler);
// }