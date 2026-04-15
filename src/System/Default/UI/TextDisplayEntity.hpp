#pragma once

#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "System/Components/Registry/UIEntity.hpp"
#include "System/Components/UI/TextRenderComponent.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    class TextDisplayEntity : public UIEntity
    {
    public:
        TextDisplayEntity(UITransform transform,
            std::unique_ptr<TextRenderComponent> textRenderComp)
            : UIEntity(transform, nullptr, Layer::UI)
        {
            textRenderComp->SetGetTransform([this]() -> UITransform* {
                return GetRootTransform();
            });

            Core::Screen::ScreenTransform()->BindChild(GetRootTransform());
            this->renderComponent = (std::move(textRenderComp));
        }

        void SetText(const std::string& text)
        {
            GetTextComponent()->SetText(text);
        }

        void Update() override
        {
            if (NeedsUpdate())
            {
                rootTransform.HierarchalUpdate();
                GetTextComponent()->ReloadTextBuffer();
                isDirty = false;
                screenVersion = Core::Screen::Version();
            }
        }

        [[nodiscard]] TextRenderComponent* GetTextComponent() { return static_cast<TextRenderComponent*>(renderComponent.get()); }

    private:
        void InitializeManager() override
        {
        }
    };
} // namespace Beer::System
