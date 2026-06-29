#pragma once

#include "Core/Application/Renderer/Screen.hpp"
#include "System/Components/Registry/UIEntity.hpp"
#include "System/Components/UI/QuadTreeRenderComponent.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <print>

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
            GetTreeRenderComp()->SetRootTransform(&this->rootTransform);
            GetTreeRenderComp()->SetGetRenderItems([this]() -> std::vector<UIRenderItem> { return GetRenderItems(); });
            MarkDirty();
        }

        virtual void Update() override
        {
            if (NeedsUpdate())
            {
                UpdateDirty();
            }
        }

        void SetTreeEnabled(bool enabled)
        {
            this->enabled = enabled;
            rootTransform.SetEnabled(enabled);
            MarkDirty();
        }

    protected:
        [[nodiscard]] QuadTreeRenderComponent* GetTreeRenderComp() const
        {
            return static_cast<QuadTreeRenderComponent*>(renderComponent.get());
        }

        virtual std::vector<UIRenderItem> GetRenderItems() = 0;

        void UpdateDirty() override
        {
            UIEntity::UpdateDirty();
            GetTreeRenderComp()->UpdateQuadDraw();
        }
    };
} // namespace Beer::System
