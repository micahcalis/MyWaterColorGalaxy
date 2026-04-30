#pragma once

#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::Rendering
{
    class Material;
    class FontMaterial;
    class TextBuffer;
} // namespace Beer::Rendering

namespace Beer::System
{
    enum class QuadType
    {
        Sprite,
        Text
    };

    struct UIRenderItem
    {
    public:
        QuadType Type;
        float Depth;
        UITransform* Transform;
        bool Enabled;

        union
        {
            Rendering::Material* SpriteMat;
            struct
            {
                Rendering::FontMaterial* FontMat;
                Rendering::TextBuffer* TextBuffer;
            } Text;
        };

    public:
        UIRenderItem()
            : Type(QuadType::Sprite), Depth(0.0f), Transform(nullptr), SpriteMat(nullptr)
        {
        }

        UIRenderItem(UITransform* transform,
            Rendering::Material* spriteMaterial)
            : Type(QuadType::Sprite), Transform(transform), SpriteMat(spriteMaterial), Enabled(Transform->GetEnabled())
        {
            Depth = transform ? transform->Depth : 0.0f;
        }

        UIRenderItem(UISubEntity* subEntity,
            Rendering::Material* spriteMaterial)
            : Type(QuadType::Sprite), Transform(subEntity->GetTransform()), SpriteMat(spriteMaterial), Enabled(Transform->GetEnabled())
        {
            Depth = Transform ? Transform->Depth : 0.0f;
        }

        UIRenderItem(UITransform* transform,
            Rendering::FontMaterial* fontMat,
            Rendering::TextBuffer* textBuffer)
            : Type(QuadType::Text), Transform(transform), Enabled(Transform->GetEnabled())
        {
            Depth = Transform ? Transform->Depth : 0.0f;

            Text.FontMat = fontMat;
            Text.TextBuffer = textBuffer;
        }

        UIRenderItem(UISubEntity* subEntity,
            Rendering::FontMaterial* fontMat,
            Rendering::TextBuffer* textBuffer)
            : Type(QuadType::Text), Transform(subEntity->GetTransform()), Enabled(Transform->GetEnabled())
        {
            Depth = Transform ? Transform->Depth : 0.0f;

            Text.FontMat = fontMat;
            Text.TextBuffer = textBuffer;
        }
    };
} // namespace Beer::System
