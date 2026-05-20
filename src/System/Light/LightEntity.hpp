#pragma once

#include "ILight.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Light/PointLight.hpp"
#include "glm/fwd.hpp"
#include <memory>
namespace Beer::System

{
    class LightEntity : public GameEntity
    {
    private:
        std::unique_ptr<PointLight> pointLight = nullptr;

    public:
        LightEntity(Transform transform,
            int priority,
            glm::vec4 lightColor,
            glm::vec4 shadowColor,
            glm::vec4 ambientColor)
            : GameEntity()
        {
            this->transform = std::move(transform);

            pointLight = ILight::CreateLight<PointLight>(&this->transform,
                10,
                lightColor,
                shadowColor,
                ambientColor);
        }

        void Update() override;

    private:
        void InitializeManager() override;
    };
} // namespace Beer::System
