#include "System/Camera/Camera.hpp"
#include "CameraHandle.hpp"
#include "Core/Application/Renderer/Renderer.hpp"
#include "System/Context/IContext.hpp"
#include "System/ECS/Components/Transform.hpp"
#include "System/ECS/ECS.hpp"
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
        Transform* transform = handle.Transform;

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

    void Camera::SetPosition(glm::vec3 position)
    {
        handle.Transform->Position = position;
    }

    void Camera::SetOrientation(glm::quat rotation)
    {
        handle.Transform->Rotation = rotation;
    }

    void Camera::SetOrientation(glm::vec3 up, glm::vec3 forward)
    {
        handle.Transform->Rotation = glm::quatLookAt(forward, up);
    }

    std::unique_ptr<Camera> Camera::CreateCamera(const CameraSettings settings,
        const ContextType context)
    {
        if (cameraManager == nullptr)
        {
            throw std::runtime_error("Can't create Camera: manager nullptr");
        }

        Registry& registry = IContext::GetRegistry(context);
        Entity cameraEntity = registry.Create();
        Transform& transform = registry.AddComponent<Transform>(cameraEntity);
        CameraHandle handle = CameraHandle(registry.GetHandle(cameraEntity), &transform);

        std::unique_ptr<Camera> camera = std::make_unique<Camera>(Camera(handle,
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
