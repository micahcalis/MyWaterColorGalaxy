#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"

namespace Beer::System
{
    class GalaxyMapEntity : public QuadTreeEntity
    {
    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;
        std::shared_ptr<Rendering::Material> galaxyMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> perlinWorleyTexture = nullptr;
        std::shared_ptr<Rendering::ComputeContext> generatePerlinWorleyContext = nullptr;

        std::shared_ptr<Rendering::Texture2D> perlinTexture = nullptr;
        std::shared_ptr<Rendering::ComputeContext> generatePerlinContext = nullptr;
        std::shared_ptr<Rendering::Material> starMaterial = nullptr;
        std::unique_ptr<UISubEntity> starEntity = nullptr;

    public:
        GalaxyMapEntity(GalaxyMapBuffer* galaxyMapBuffer);

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

            if (starEntity != nullptr)
            {
                renderItems.push_back(UIRenderItem(starEntity->GetTransform(), starMaterial.get()));
            }

            return renderItems;
        }

    private:
        void InitializePerlinWorleyTex();
        void InitializeStar();
        void InitializePerlinTex();
        void UpdateStarPosition(glm::vec2 position);
    };
} // namespace Beer::System
