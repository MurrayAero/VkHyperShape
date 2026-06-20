// #ifndef CAMERA_H
// #define CAMERA_H
// // #include <glm/glm.hpp>
// // #include <glm/gtc/quaternion.hpp>

// #include "VulkanBuffer.h"

// #define MIN_CAMERA_ZOOM 0.0f
// #define MAX_CAMERA_ZOOM 45.0f
// struct Camera {
//     VulkanBuffer uniform;

//     Camera(float distance = 1.0f) :m_distance(distance), m_yaw(0.0f), m_pitch(0.0f), m_zoom(MAX_CAMERA_ZOOM), m_center(0.0f){
//         UpdateRotation();  // 初始化旋转状态
//         UpdatePosition();  // 计算初始位置
//     }
//     void CreateUniform(const VulkanDevice*device){
//         uniform = CreateBuffer(device, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
//     }
//     void DestroyUniform(VkDevice device){
//         Destroy(device, uniform);
//     }
    
//     glm::mat4 GetPerspective(float aspect){
//         return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 100.0f);
//     }
    
//     glm::vec3 GetCenter(){
//         return m_center;
//     }
    
//     glm::vec3 GetPosition(){
//         return m_position;
//     }
    
//     glm::mat4 GetView(){
//         return m_view;
//     }
    
//     void ProcessMouseScroll(float yoffset){
//         m_zoom = glm::clamp(m_zoom - yoffset, MIN_CAMERA_ZOOM, MAX_CAMERA_ZOOM);
//     }
//     void UpdateUniform(VkDevice device){
//         UpdateViewMatrix();
//         uniform.UpdateData(device, &m_view);
//     }
//     void UpdateRotation(float deltaYaw, float deltaPitch) {
//         m_yaw += deltaYaw;
//         m_pitch += deltaPitch;
        
//         // 限制俯仰角范围 (-89° ~ 89°)
//         m_pitch = glm::clamp(m_pitch, -1.57f, 1.57f);
        
//         UpdateRotation();
//         UpdatePosition();
//     }

//     void MoveCenter(const glm::vec3& offset) {
//         m_center += offset;
//         UpdatePosition();
//     }

//     void SetCenter(const glm::vec3& center) {
//         m_center = center;
//         UpdatePosition();
//     }
//     float m_zoom;
//     float m_distance;
//     float m_yaw, m_pitch;
//     glm::vec3 m_position;
//     glm::vec3 m_center;
//     glm::quat m_rotation;
//     glm::mat4 m_view;
    
//     void UpdateRotation() {
//         glm::quat pitchQuat = glm::angleAxis(m_pitch, glm::vec3(1, 0, 0));
//         glm::quat yawQuat = glm::angleAxis(m_yaw, glm::vec3(0, 1, 0));
//         m_rotation = yawQuat * pitchQuat;
//     }
//     void UpdatePosition() {
//         m_position = m_center - (m_rotation * glm::vec3(0, m_distance, m_distance));
//         UpdateViewMatrix();
//     }
//     void UpdateViewMatrix() {
//         const glm::vec3 forward = glm::normalize(m_center - m_position);
//         const glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
//         const glm::vec3 up = glm::normalize(glm::cross(right, forward));
        
//         m_view = glm::lookAt(m_position, m_center, up);
//     }
// };
// #endif