#pragma once

#include "CameraSettings.hpp"
#include "System/Camera/CameraType.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Components/General/Transform.hpp"
#include <cstdint>
#include <memory>

namespace Beer::System
{
    class CameraManager;
}

namespace Beer::System
{
    class Camera
    {
    private:
        Transform* transform;
        CameraType type{};
        float fieldOfView{};
        float nearMin{};
        float farMax{};
        uint32_t priority{};

    public:
        uint32_t GetPriority() const;
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        const Transform* GetTransform();
        void SetPosition(glm::vec3 position);
        void SetOrientation(glm::quat rotation);
        void SetOrientation(glm::vec3 up, glm::vec3 forward);
        void SetPriority(uint32_t priority);
        void BindToShaders();

    private:
        Camera(Transform* transform,
            CameraType type,
            float fieldOfView,
            float nearMin,
            float farMax,
            uint32_t priority)
            : transform(transform), type(type), fieldOfView(fieldOfView), nearMin(nearMin), farMax(farMax), priority(priority)
        {
        }

    private:
        inline static CameraManager* cameraManager = nullptr;

    public:
        static std::unique_ptr<Camera> CreateCamera(const CameraSettings settings, Transform* transform);
        static float GetAspect();
        static void SetCameraManager(CameraManager* cameraManager) { Camera::cameraManager = cameraManager; }
        static Camera* Main();
    };
} // namespace Beer::System
