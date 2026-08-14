#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vulkan/buffer.hpp"

class Camera {
public:
    Camera(float distance = 1.0f) : m_distance(distance), m_center(0.0f), m_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)){
        UpdatePosition();
    }

    void CreateUniform(const vulkan::Device& device) {
        m_uniform.Create(device, sizeof(glm::mat4), vk::BufferUsageFlagBits::eUniformBuffer, vma::MemoryUsage::eCpuOnly, true);
    }

    void DestroyUniform(const vulkan::Device& device) {
        m_uniform.Destroy(device);
    }

    void ProcessMouseMovement(float deltaX, float deltaY, float sensitivity = 0.005f) {
        if (deltaX == 0.0f && deltaY == 0.0f) return;

        // 构造鼠标移动的旋转增量（在当前局部坐标系中）
        // 先俯仰（绕X轴），再偏航（绕Y轴），都在局部空间
        glm::quat pitchQuat = glm::angleAxis(-deltaY * sensitivity, glm::vec3(1, 0, 0));
        glm::quat yawQuat = glm::angleAxis(-deltaX * sensitivity, glm::vec3(0, 1, 0));

        glm::quat deltaRot = yawQuat * pitchQuat;

        m_rotation = m_rotation * deltaRot;

        m_rotation = glm::normalize(m_rotation);
        UpdatePosition();
    }

    void ProcessRoll(float deltaRoll, float sensitivity = 0.005f) {
        if (deltaRoll == 0.0f) return;

        const glm::vec3 forward = m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::quat rollRot = glm::angleAxis(deltaRoll * sensitivity, forward);

        m_rotation = rollRot * m_rotation;
        m_rotation = glm::normalize(m_rotation);

        UpdatePosition();
    }

    void ProcessMouseScroll(float yoffset, float minDist = 0.1f, float maxDist = 100.0f) {
        m_distance = glm::clamp(m_distance - yoffset * 0.1f, minDist, maxDist);
        UpdatePosition();
    }

    void Pan(const glm::vec2& delta, float sensitivity = 0.01f) {
        const glm::vec3 right = m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 up = m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);

        m_center += right * delta.x * sensitivity * m_distance;
        m_center -= up * delta.y * sensitivity * m_distance;

        UpdatePosition();
    }

    void MoveCenter(const glm::vec3& offset) {
        m_center += offset;
        UpdatePosition();
    }

    void SetCenter(const glm::vec3& center) {
        m_center = center;
        UpdatePosition();
    }

    void SetDistance(float distance) {
        m_distance = distance;
        UpdatePosition();
    }

    void SetRotation(const glm::quat& rotation) {
        m_rotation = glm::normalize(rotation);
        UpdatePosition();
    }

    glm::mat4 GetView() const {
        return m_view;
    }

    glm::vec3 GetPosition() const {
        return m_position;
    }

    glm::vec3 GetCenter() const {
        return m_center;
    }

    glm::quat GetRotation() const {
        return m_rotation;
    }

    float GetDistance() const {
        return m_distance;
    }

    glm::vec3 GetRight() const { return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
    glm::vec3 GetUp() const { return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f); }
    glm::vec3 GetForward() const { return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f); }

    const auto& GetUniform() const noexcept {
        return m_uniform;
    }

    void UpdateUniform(const vulkan::Device& device) {
        m_uniform.UpdateData(device, &m_view);
    }

private:
    glm::quat m_rotation;// 摄像机朝向
    glm::vec3 m_center;
    float m_distance;

    glm::vec3 m_position;
    glm::mat4 m_view;

    vulkan::Buffer m_uniform;

    void UpdatePosition() {
        // 从四元数计算"后"方向（摄像机在中心后方）
        // m_rotation * (0,0,-1) 表示旋转后的"前"方向，取反得到从center到position的方向
        const glm::vec3 back = m_rotation * glm::vec3(0.0f, 0.0f, 1.0f);
        m_position = m_center + back * m_distance;

        UpdateViewMatrix();
    }

    void UpdateViewMatrix() {
        const glm::vec3 up = m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        m_view = glm::lookAt(m_position, m_center, up);
    }
};
#endif