#pragma once

#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class UIRenderComponent : public IRenderComponent
    {
    public:
        virtual ~UIRenderComponent() = default;
        virtual float GetDepth() const = 0;

        RenderCompType GetType() const override
        {
            return RenderCompType::Sprite;
        }
    };
} // namespace Beer::System
