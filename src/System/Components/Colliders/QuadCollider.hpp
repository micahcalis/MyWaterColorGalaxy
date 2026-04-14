#pragma once

#include "System/Components/UI/UITransform.hpp"
#include "glm/fwd.hpp"

namespace Beer::System
{
    class QuadCollider
    {
        friend class QuadColliderRegistry;

    private:
        UITransform* transform;
        bool enabled = true;

    private:
        QuadCollider(UITransform* transform)
            : transform(transform)
        {
        }

    public:
        ~QuadCollider();
        void SetEnabled(bool enabled)
        {
            this->enabled = enabled;
        }

        bool Hit(glm::vec2 pixelPosition) const
        {
            if (!enabled)
            {
                return false;
            }

            const PixelRect& rect = transform->Rect;

            return pixelPosition.x >= rect.BotLeft.x
                && pixelPosition.y >= rect.BotLeft.y
                && pixelPosition.x < rect.TopRight.x
                && pixelPosition.y < rect.TopRight.y;
        }

        float GetDepth() const
        {
            return transform->Depth;
        }
    };
} // namespace Beer::System
