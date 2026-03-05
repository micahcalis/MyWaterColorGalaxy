#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Beer::System
{
    struct Transform
    {
    public:
        glm::vec3 Position;
        glm::quat Rotation;
        glm::vec3 Scale;

    public:
        glm::vec3 GetForward() const
        {
            return Rotation * glm::vec3(0, 0, 1);
        }

        glm::vec3 GetRight() const
        {
            return Rotation * glm::vec3(1, 0, 0);
        }

        glm::vec3 GetUp() const
        {
            return Rotation * glm::vec3(0, 1, 0);
        }
    };
} // namespace Beer::System
