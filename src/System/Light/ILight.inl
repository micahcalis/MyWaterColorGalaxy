#pragma once
#include "ILight.hpp"
#include "LightManager.hpp"
#include <utility>
#include <memory>

namespace Beer::System
{
    inline ILight::~ILight()
    {
        if (lightManager != nullptr)
        {
            lightManager->RemoveLight(this);
        }
    }

    template<typename T, typename... Args>
    std::unique_ptr<T> ILight::CreateLight(Args&&... args)
    {
        auto newLight = std::make_unique<T>(std::forward<Args>(args)...);

        if (lightManager != nullptr)
        {
            lightManager->AddLight(newLight.get());
        }

        return newLight;
    }

    inline ILight* ILight::Main()
    {
        return lightManager->GetMainLight();
    }
} // namespace Beer::System
