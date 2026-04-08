#pragma once

#include "System/Components/UI/UIRect.hpp"
#include "glm/glm.hpp"
#include "Rendering/Shader/ModelPush.hpp"

namespace Beer::System
{
    struct UITransform
    {
    public:
        glm::vec2 Position = glm::vec2(0);
        float Depth = 0;
        float Rotation = 0;
        glm::vec2 Scale = glm::vec2(1);

        UITransform(glm::vec2 position, float depth, float rotation, glm::vec2 scale)
            : Position(position), Depth(depth), Rotation(rotation), Scale(scale)
        {
        }

    public:
        Rendering::RectPush GetRectPush(UIRect rect)
        {
            return {Position,
                Scale,
                Depth,
                Rotation,
                rect.Mode};
        }
    };
} // namespace Beer::System
