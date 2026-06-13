#pragma once

#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <print>
namespace Beer::System
{
    struct TextTransform : public UITransform
    {
    public:
        std::shared_ptr<Rendering::FontMaterial> FontMaterial = nullptr;
        std::shared_ptr<Rendering::TextBuffer> TextBuffer = nullptr;
        TextSettings TextSettings{};

        void HierarchalUpdate() override
        {
            CalculatePixelRect();

            if (FontMaterial != nullptr && TextBuffer != nullptr)
            {
                TextBuffer->Reload(FontMaterial->GetAsset(),
                    FontMaterial->GetSettings(),
                    TextSettings,
                    GetUITransform());
            }

            for (auto& child : children)
            {
                child->SetEnabledInHierarchy(this->GetEnabled());
                child->HierarchalUpdate();
            }
        }

        UITransform* GetUITransform() { return static_cast<UITransform*>(this); }
    };
} // namespace Beer::System
