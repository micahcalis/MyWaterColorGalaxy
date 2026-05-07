#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"

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

        Function<MouseInput> getMouseInput = nullptr;
        Function<bool> isColorMixerOpen = nullptr;

    public:
        GalaxyMapEntity(GalaxyMapBuffer* galaxyMapBuffer,
            Function<MouseInput> getMouseInput,
            Function<bool> isColorPickerOpen);

        void Update() override
        {
            if (!isColorMixerOpen())
                manager->Update();

            QuadTreeEntity::Update();
        }

        GalaxyMapManager* GetMapManager() const { return static_cast<GalaxyMapManager*>(manager.get()); }
        void InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor);

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<GalaxyMapManager>(galaxyMapBuffer,
                &rootTransform,
                getMouseInput);
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, galaxyMaterial.get()));

            if (starEntity != nullptr)
            {
                renderItems.push_back(UIRenderItem(starEntity->GetTransform(), starMaterial.get()));
            }

            renderItems.append_range(galaxyMapBuffer->GetRenderItems());

            return renderItems;
        }

    private:
        void InitializePerlinWorleyTex();
        void InitializeStar();
        void InitializePerlinTex();
        void UpdateStarPosition(glm::vec2 position);
    };
} // namespace Beer::System
