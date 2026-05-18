#include "System/Light/LightManager.hpp"
#include "ILight.hpp"
#include <algorithm>

namespace Beer::System
{
    void LightManager::Update()
    {
        SortLights();
    }

    void LightManager::AddLight(ILight* light)
    {
        for (const auto existingLight : lights)
        {
            if (existingLight == light)
                return;
        }

        lights.push_back(light);
    }

    void LightManager::RemoveLight(ILight* light)
    {
        std::erase(lights, light);
    }

    ILight* LightManager::GetMainLight() const
    {
        if (lights.size() == 0)
            return nullptr;

        return lights[0];
    }

    void LightManager::SortLights()
    {
        std::erase_if(lights, [](const ILight* camera) {
            return camera == nullptr;
        });

        std::sort(lights.begin(), lights.end(), [](const ILight* a, const ILight* b) {
            return a->GetPriority() > b->GetPriority();
        });
    }
} // namespace Beer::System
