#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    class GalaxyMapEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> galaxyMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> perlinWorleyTexture = nullptr;
        std::shared_ptr<Rendering::ComputeContext> generatePerlinWorleyContext = nullptr;

    public:
        GalaxyMapEntity();

        void Update() override
        {
            QuadTreeEntity::Update();
        }

    protected:
        void InitializeManager() override
        {
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, galaxyMaterial.get()));
            return renderItems;
        }
    };
} // namespace Beer::System
