#pragma once

#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "glm/fwd.hpp"

namespace Beer::System
{
    class QuadCollider
    {
        friend class QuadColliderRegistry;

    private:
        UITransform* transform;
        bool enabled = true;

        Function<void, const MouseInput> onColliderEntry;
        Function<void, const MouseInput> onColliderActive;
        Function<void, const MouseInput> onColliderExit;

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

        void OnColliderEntry(const MouseInput input)
        {
            if (onColliderEntry == nullptr)
                return;

            onColliderEntry(input);
        }

        void OnColliderActive(const MouseInput input)
        {
            if (onColliderActive == nullptr)
                return;

            onColliderActive(input);
        }

        void OnColliderExit(const MouseInput input)
        {
            if (onColliderExit == nullptr)
                return;

            onColliderExit(input);
        }

        void SetColliderEntry(Function<void, const MouseInput> onColliderEntry)
        {
            this->onColliderEntry = onColliderEntry;
        }

        void SetColliderActive(Function<void, const MouseInput> onColliderActive)
        {
            this->onColliderActive = onColliderActive;
        }

        void SetColliderExit(Function<void, const MouseInput> onColliderExit)
        {
            this->onColliderExit = onColliderExit;
        }

        bool Hit(glm::vec2 pixelPosition) const
        {
            if (!enabled || !transform->GetEnabled())
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

        static bool Hit(const UITransform* transform, glm::vec2 pixelPosition)
        {
            if (!transform->GetEnabled())
            {
                return false;
            }

            const PixelRect& rect = transform->Rect;

            return pixelPosition.x >= rect.BotLeft.x
                && pixelPosition.y >= rect.BotLeft.y
                && pixelPosition.x < rect.TopRight.x
                && pixelPosition.y < rect.TopRight.y;
        }
    };
} // namespace Beer::System
