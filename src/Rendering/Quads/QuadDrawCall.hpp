#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include <print>

namespace Beer::Rendering
{
    class QuadDrawCall
    {
    private:
        Material* spriteMaterial = nullptr;
        uint32_t firstIndex = 0;
        uint32_t indexCount = 0;

        FontMaterial* fontMaterial = nullptr;
        TextBuffer* textBuffer = nullptr;
        bool isText;

    public:
        QuadDrawCall(Material* spriteMaterial,
            uint32_t firstIndex,
            uint32_t indexCount)
            : isText(false), spriteMaterial(spriteMaterial), firstIndex(firstIndex), indexCount(indexCount)
        {
        }

        QuadDrawCall(FontMaterial* fontMaterial,
            TextBuffer* textBuffer)
            : isText(true), fontMaterial(fontMaterial), textBuffer(textBuffer)
        {
        }

        bool IsText() const { return isText; }

        void Execute(CommandBuffer* commandBuffer,
            const RenderContext& context,
            const ShaderPassType pass) const;

        void Print() const
        {
            std::println("Is Quad Draw Call: {}", !IsText());

            if (!IsText())
            {
                std::println("First Index: {}", firstIndex);
                std::println("Index Count: {}", indexCount);
            }

            std::println("Draw Call end");
        }

    private:
        void DrawQuads(CommandBuffer* commandBuffer,
            const RenderContext& context,
            const ShaderPassType pass) const;

        void DrawText(CommandBuffer* commandBuffer,
            const RenderContext& context,
            const ShaderPassType pass) const;
    };
} // namespace Beer::Rendering
