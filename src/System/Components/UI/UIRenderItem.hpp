#pragma once

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
            : Type(QuadType::Sprite), Transform(transform), SpriteMat(spriteMaterial)
        {
            Depth = transform ? transform->Depth : 0.0f;
        }

        UIRenderItem(UITransform* transform,
            Rendering::FontMaterial* fontMat,
            Rendering::TextBuffer* textBuffer)
            : Type(QuadType::Text), Transform(transform)
        {
            Depth = transform ? transform->Depth : 0.0f;

            Text.FontMat = fontMat;
            Text.TextBuffer = textBuffer;
        }
    };
} // namespace Beer::System
