#pragma once

#include "System/Base/Clock/Clock.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstdint>

namespace Beer::System
{
    enum class OrbitDirection : int32_t
    {
        ClockWise = 1,
        CounterClockWise = -1
    };

    class OrbitComponent
    {
    private:
        Transform* transform = nullptr;
        float speed = 0;
        float radius = 0;
        OrbitDirection direction = OrbitDirection::ClockWise;
        glm::vec3 center = glm::vec3(0);
        glm::vec2 shear = glm::vec2(1);
        glm::vec2 tilt = glm::vec2(0);

        glm::vec3 startPos = glm::vec3(0);
        float initialAngle = 0.0f;

    public:
        OrbitComponent(Transform* transform,
            float speed,
            OrbitDirection direction,
            glm::vec3 center,
            glm::vec2 shear,
            glm::vec2 tilt,
            glm::vec3 startPos)
            : transform(transform)
            , speed(speed)
            , direction(direction)
            , center(center)
            , shear(shear)
            , tilt(tilt)
        {
            startPos = transform->Position;
            float dx = startPos.x - center.x;
            float dz = startPos.z - center.z;
            radius = std::sqrt(dx * dx + dz * dz);
            initialAngle = std::atan2(dz, dx);
        }

        void Update()
        {
            float timeAngle = speed * Clock::Time() * static_cast<int32_t>(direction);
            float currentAngle = initialAngle + timeAngle;

            float flatX = std::cos(currentAngle) * radius * shear.x;
            float flatZ = std::sin(currentAngle) * radius * shear.y;

            float offsetY = (flatZ * std::sin(tilt.x)) + (flatX * std::sin(tilt.y));

            float finalX = flatX * std::cos(tilt.y);
            float finalZ = flatZ * std::cos(tilt.x);

            transform->Position.x = center.x + finalX;
            transform->Position.y = center.y + offsetY;
            transform->Position.z = center.z + finalZ;
        }
    };
} // namespace Beer::System
