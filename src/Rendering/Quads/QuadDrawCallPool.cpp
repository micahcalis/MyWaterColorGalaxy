#include "Rendering/Quads/QuadDrawCallPool.hpp"

namespace Beer::Rendering
{
    static constexpr int QUAD_INDICES = 6;

    void QuadDrawCallPool::Update(std::vector<System::UIRenderItem> renderItems)
    {
        SortRenderItems(renderItems);
        BuildDrawCalls(renderItems);
    }

    void QuadDrawCallPool::SortRenderItems(std::vector<System::UIRenderItem>& renderItems)
    {
        std::sort(renderItems.begin(),
            renderItems.end(),
            [](const System::UIRenderItem& a, const System::UIRenderItem& b) {
                return a.Depth < b.Depth;
            });
    }

    void QuadDrawCallPool::BuildDrawCalls(std::vector<System::UIRenderItem>& renderItems)
    {
        quadBuffer->Clear();
        drawCalls.clear();

        Material* currentSpriteMat = nullptr;
        uint32_t currentFirstIndex = 0;
        uint32_t currentIndexCount = 0;

        for (const auto& item : renderItems)
        {
            if (item.Type == System::QuadType::Sprite)
            {
                if (item.SpriteMat != currentSpriteMat)
                {
                    CreateSpriteDrawCall(currentSpriteMat, currentFirstIndex, currentIndexCount);

                    currentSpriteMat = item.SpriteMat;
                    currentIndexCount = 0;
                }

                uint32_t quadIndex = quadBuffer->AddQuad(item.Transform->Rect);
                if (currentIndexCount == 0)
                {
                    currentFirstIndex = quadIndex;
                }

                currentIndexCount += QUAD_INDICES;
            } else if (item.Type == System::QuadType::Text)
            {
                CreateSpriteDrawCall(currentSpriteMat, currentFirstIndex, currentIndexCount);

                currentSpriteMat = nullptr;
                currentIndexCount = 0;

                CreateTextDrawCall(item.Text.FontMat, item.Text.TextBuffer);
            }
        }

        CreateSpriteDrawCall(currentSpriteMat, currentFirstIndex, currentIndexCount);

        quadBuffer->Flush();
    }

    void QuadDrawCallPool::CreateSpriteDrawCall(Material* material, uint32_t firstIndex, uint32_t indexCount)
    {
        if (indexCount > 0 && material != nullptr)
        {
            drawCalls.emplace_back(material, firstIndex, indexCount);
        }
    }

    void QuadDrawCallPool::CreateTextDrawCall(FontMaterial* fontMaterial, TextBuffer* textBuffer)
    {
        if (fontMaterial != nullptr && textBuffer != nullptr)
        {
            drawCalls.emplace_back(fontMaterial, textBuffer);
        }
    }

} // namespace Beer::Rendering
