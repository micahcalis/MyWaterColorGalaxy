#pragma once
#include "Core/Application/Renderer/Screen.hpp"
#include "System/Components/Registry/IEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class UIEntity : public IEntity
    {
    protected:
        UITransform uiTransform;

    public:
        virtual ~UIEntity() = default;

        [[nodiscard]] UITransform* GetUITransform() { return &uiTransform; }

    protected:
        UIEntity(UITransform uiTransform,
            std::unique_ptr<IRenderComponent> renderComponent,
            Layer layer = Layer::UI)
            : uiTransform(uiTransform), IEntity(std::move(renderComponent), layer)
        {
            //  uiTransform.Parent = Core::Screen::ScreenTransform();
        }
    };
} // namespace Beer::System
