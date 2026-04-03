#pragma once

#include "MulitpleContainerManager.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Default/MulitpleContainerManager.hpp"

namespace Beer::System
{
    class RotateEntitiesManager : public MultipleContainerManager
    {
    private:
        float radius;
        float speed;

    public:
        RotateEntitiesManager(float radius, float speed)
            : radius(radius), speed(speed)
        {
        }

    protected:
        void UpdateSubEntity(GameSubEntity& subEntity, uint32_t index, uint32_t entityCount) override
        {
            float fraction = static_cast<float>(index) / static_cast<float>(entityCount);
            float phaseOffset = fraction * glm::two_pi<float>();

            float currentAngle = phaseOffset + (Clock::Time() * speed);

            float offsetX = glm::cos(currentAngle) * radius;
            float offsetZ = glm::sin(currentAngle) * radius;

            Transform* transform = subEntity.GetTransform();
            transform->Position = parent->Position + glm::vec3(offsetX, 0.0f, offsetZ);
        }
    };
} // namespace Beer::System
