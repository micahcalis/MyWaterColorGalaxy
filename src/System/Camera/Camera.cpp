#include "System/Camera/Camera.hpp"
#include "Core/Application/Renderer/Renderer.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "vulkan/vulkan.hpp"
#include "System/Camera/CameraManager.hpp"
#include <memory>
#include <stdexcept>

namespace Beer::System
{
    uint32_t Camera::GetPriority() const
    {
        return priority;
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(transform->Position,
            transform->Position + transform->GetForward(),
            transform->GetUp());
    }

    glm::mat4 Camera::GetProjectionMatrix() const
    {
        glm::mat4 projMat = glm::perspective(glm::radians(fieldOfView),
            GetAspect(),
            nearMin,
            farMax);

        projMat[1][1] *= -1;
        return projMat;
    }

    const Transform* Camera::GetTransform() const
    {
        return transform;
    }

    void Camera::SetPosition(glm::vec3 position)
    {
        transform->Position = position;
    }

    void Camera::SetOrientation(glm::quat rotation)
    {
        transform->Rotation = rotation;
    }

    void Camera::SetOrientation(glm::vec3 up, glm::vec3 forward)
    {
        transform->Rotation = glm::quatLookAt(forward, up);
    }

    void Camera::BindToShaders()
    {
        Rendering::Shader::Globals()->SetCamera(GetViewMatrix(), GetProjectionMatrix(), GetTransform()->Position, GetTransform()->GetForward());
        Rendering::Shader::Globals()->SetZBuffer(nearMin, farMax);
    }

    std::unique_ptr<Camera> Camera::CreateCamera(const CameraSettings settings,
        Transform* transform)
    {
        if (cameraManager == nullptr)
        {
            throw std::runtime_error("Can't create Camera: manager nullptr");
        }

        std::unique_ptr<Camera> camera = std::make_unique<Camera>(Camera(transform,
            settings.type,
            settings.fieldOfView,
            settings.nearMin,
            settings.farMax,
            settings.priority));

        cameraManager->AddCamera(camera.get());
        return std::move(camera);
    }

    float Camera::GetAspect()
    {
        vk::Extent2D extent = Core::Renderer::GetScreenExtent();

        if (extent.height == 0)
            return 1;

        return static_cast<float>(extent.width) / static_cast<float>(extent.height);
    }

    Camera* Camera::Main()
    {
        if (cameraManager == nullptr)
            return nullptr;

        return cameraManager->GetMainCamera();
    }
} // namespace Beer::System
