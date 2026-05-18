#pragma once

#include "Camera.hpp"

namespace Beer::System
{
    class CameraManager
    {
    private:
        std::vector<Camera*> cameras;

    public:
        void Update();
        void AddCamera(Camera* camera);
        void RemoveCamera(Camera* camera);
        Camera* GetMainCamera() const;

    private:
        void SortCameras();
    };
} // namespace Beer::System
