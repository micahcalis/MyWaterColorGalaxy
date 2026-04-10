#pragma once

#include "Core/Application/Renderer/Screen.hpp"
#include "System/Components/Registry/UIEntity.hpp"
#include "System/Components/UI/QuadTreeRenderComponent.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class QuadTreeEntity : public UIEntity
    {
    public:
        virtual ~QuadTreeEntity() = default;
        QuadTreeEntity(UITransform rootTransform,
            std::unique_ptr<QuadTreeRenderComponent> quadTreeRenderComp)
            : UIEntity(rootTransform, std::move(quadTreeRenderComp))
        {
            Core::Screen::ScreenTransform()->BindChild(&this->rootTransform);
            GetTreeRenderComp()->SetGetRenderItems([this]() -> std::vector<UIRenderItem> { return GetRenderItems(); });
        }

        virtual void Update() override
        {
            rootTransform.HierarchalUpdate();
            GetTreeRenderComp()->UpdateQuadDraw();
        }

    protected:
        [[nodiscard]] QuadTreeRenderComponent* GetTreeRenderComp() const { return static_cast<QuadTreeRenderComponent*>(renderComponent.get()); }
        virtual std::vector<UIRenderItem> GetRenderItems() = 0;
    };
} // namespace Beer::System
