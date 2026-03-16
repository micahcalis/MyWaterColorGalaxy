#include "System/Camera/CameraManager.hpp"
#include "CameraManager.hpp"

namespace Beer::System
{
    void CameraManager::Update()
    {
        SortCameras();
    }

    void CameraManager::AddCamera(Camera* camera)
    {
        for (const auto existingCam : cameras)
        {
            if (existingCam == camera)
                return;
        }

        cameras.push_back(camera);
    }

    Camera* CameraManager::GetMainCamera() const
    {
        if (cameras.size() == 0)
            return nullptr;

        return cameras[0];
    }

    void CameraManager::SortCameras()
    {
        std::erase_if(cameras, [](const Camera* camera) {
            return camera == nullptr;
        });

        std::sort(cameras.begin(), cameras.end(), [](const Camera* a, const Camera* b) {
            return a->GetPriority() > b->GetPriority();
        });
    }
} // namespace Beer::System
