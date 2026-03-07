#pragma once

#include "CameraHandle.hpp"
#include "CameraSettings.hpp"
#include "System/Camera/CameraType.hpp"
#include "System/Context/ContextType.hpp"
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
        CameraHandle handle;
        CameraType type;
        float fieldOfView;
        float nearMin;
        float farMax;
        uint32_t priority;

    public:
        uint32_t GetPriority() const;
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        const Transform* GetTransform() const { return handle.Transform; }
        void SetPosition(glm::vec3 position);
        void SetOrientation(glm::quat rotation);
        void SetOrientation(glm::vec3 up, glm::vec3 forward);
        void SetPriority(uint32_t priority);

    private:
        Camera(CameraHandle handle,
            CameraType type,
            float fieldOfView,
            float nearMin,
            float farMax,
            uint32_t priority)
            : handle(handle), type(type), fieldOfView(fieldOfView), nearMin(nearMin), farMax(farMax), priority(priority)
        {
        }

    private:
        inline static CameraManager* cameraManager = nullptr;

    public:
        static std::unique_ptr<Camera> CreateCamera(const CameraSettings settings, const ContextType context);
        static float GetAspect();
        static void SetCameraManager(CameraManager* cameraManager) { Camera::cameraManager = cameraManager; }
        static Camera* Main();
    };
} // namespace Beer::System
