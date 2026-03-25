#pragma once

#include "System/Light/ILight.hpp"
#include "System/Components/General/Transform.hpp"

namespace Beer::System
{
    class PointLight : public ILight
    {
    private:
        Transform* transform;
        int priority;
        glm::vec4 lightColor;
        glm::vec4 shadowColor;
        glm::vec4 ambientColor;

    public:
        PointLight(Transform* transform,
            int priority,
            glm::vec4 lightColor,
            glm::vec4 shadowColor,
            glm::vec4 ambientColor)
            : transform(transform), priority(priority), lightColor(lightColor), shadowColor(shadowColor), ambientColor(ambientColor)
        {
        }

        glm::vec3 GetPosition() const override { return transform->Position; }
        glm::vec4 GetDirectColor() const override { return lightColor; }
        glm::vec4 GetShadowColor() const override { return shadowColor; }
        glm::vec4 GetAmbientColor() const override { return ambientColor; }
        int GetPriority() const override { return priority; }
    };
} // namespace Beer::System
