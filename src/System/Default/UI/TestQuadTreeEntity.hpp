#pragma once

#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "System/Components/UI/QuadTreeRenderComponent.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/Components/UI/UISubEntity.hpp"

namespace Beer::System
{
    class TestQuadTreeEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::FontMaterial> fontMaterial;
        std::shared_ptr<Rendering::TextBuffer> textBuffer;
        TextSettings textSettings{};

        std::shared_ptr<Rendering::Material> quadMaterial;
        std::vector<UISubEntity> diagonalChainEntities;

    public:
        TestQuadTreeEntity(UITransform initialTransform)
            : QuadTreeEntity(initialTransform, RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::Galaxy))
        {
            fontMaterial = std::make_shared<Rendering::FontMaterial>("MirandaSans");
            textBuffer = std::make_shared<Rendering::TextBuffer>();

            textBuffer->Update("Text test prrr",
                fontMaterial->GetAsset(),
                fontMaterial->GetSettings(),
                textSettings,
                &rootTransform);

            quadMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");

            UITransform* previousTransform = &rootTransform;
            uint32_t testCount = 10;

            diagonalChainEntities.reserve(testCount);

            for (size_t i = 0; i < testCount; i++)
            {
                UITransform newTransform{};
                newTransform.Scale = glm::vec2(1, 1);
                newTransform.Anchor = AnchorMode::Center;
                newTransform.Pivot = AnchorMode::BottomLeft;
                diagonalChainEntities.emplace_back(UISubEntity(std::move(newTransform)));
                previousTransform->BindChild(diagonalChainEntities[i].GetTransform());
                previousTransform = diagonalChainEntities[i].GetTransform();
            }
        }

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, quadMaterial.get()));

            for (size_t i = 0; i < diagonalChainEntities.size(); i++)
            {
                renderItems.push_back(UIRenderItem(diagonalChainEntities[i].GetTransform(), quadMaterial.get()));
            }

            return renderItems;
        }
    };
} // namespace Beer::System
