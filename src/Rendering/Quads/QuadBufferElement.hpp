#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::Rendering
{
    class QuadBufferElement
    {
    private:
        System::UITransform* transform;
        Material* material;
        FontMaterial* fontMaterial;
        TextBuffer* textBuffer;
        const bool isText;

    public:
        QuadBufferElement(System::UITransform* transform,
            Material* material)
            : isText(false), transform(transform), material(material)
        {
        }

        QuadBufferElement(System::UITransform* transform,
            FontMaterial* fontMaterial,
            TextBuffer* textBuffer)
            : isText(true), transform(transform), fontMaterial(fontMaterial), textBuffer(textBuffer)
        {
        }

    public:
        bool IsText() const { return isText; }
    };
} // namespace Beer::Rendering
