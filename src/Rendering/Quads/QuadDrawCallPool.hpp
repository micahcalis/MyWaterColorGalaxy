#pragma once

#include "System/Components/UI/UIRenderItem.hpp"
#include "Rendering/Quads/QuadDrawCall.hpp"
#include "Rendering/Quads/QuadBuffer.hpp"
#include <vector>

namespace Beer::Rendering
{
    class QuadDrawCallPool
    {
    private:
        std::vector<QuadDrawCall> drawCalls;
        QuadBuffer* quadBuffer;

    public:
        QuadDrawCallPool(QuadBuffer* quadBuffer)
            : quadBuffer(quadBuffer)
        {
        }

        void Update(std::vector<System::UIRenderItem> renderItems);
        [[nodiscard]] std::vector<QuadDrawCall>& GetDrawCalls() { return drawCalls; }

    private:
        void SortRenderItems(std::vector<System::UIRenderItem>& renderItems);
        void BuildDrawCalls(std::vector<System::UIRenderItem>& renderItems);
        void CreateSpriteDrawCall(Material* material, uint32_t firstIndex, uint32_t indexCount);
        void CreateTextDrawCall(FontMaterial* fontMaterial, TextBuffer* textBuffer);
    };
} // namespace Beer::Rendering
