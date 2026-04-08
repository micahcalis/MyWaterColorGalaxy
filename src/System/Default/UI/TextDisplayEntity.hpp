#pragma once

#include "Rendering/Shader/ModelPush.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Components/UI/TextRenderComponent.hpp"
#include "System/Components/UI/UIRect.hpp"
#include <memory>

namespace Beer::System
{
    class TextDisplayEntity : public GameEntity
    {
    private:
        UIRect rect;

    public:
        TextDisplayEntity(Transform transform,
            UIRect rect,
            std::unique_ptr<TextRenderComponent> textRenderComp)
            : GameEntity(transform, nullptr, Layer::UI), rect(rect)
        {
            textRenderComp->SetGetRectPush([this, rect, transform]() -> Rendering::RectPush {
                return transform.GetUITransform().GetRectPush(rect);
            });

            this->renderComponent = (std::move(textRenderComp));
        }

        void SetText(const std::string& text)
        {
            TextRenderComponent* textRenderComp = static_cast<TextRenderComponent*>(renderComponent.get());
            textRenderComp->SetText(text);
        }

        void Update() override
        {
        }

    private:
        void InitializeManager() override
        {
        }
    };
} // namespace Beer::System
