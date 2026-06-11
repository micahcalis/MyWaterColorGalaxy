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
    public:
        static constexpr float CENTER_STAR_SCALE = 0.15f;

    private:
        GalaxyMapBuffer* galaxyMapBuffer = nullptr;
        std::shared_ptr<Rendering::Material> galaxyMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> perlinWorleyTexture = nullptr;
        std::shared_ptr<Rendering::ComputeContext> generatePerlinWorleyContext = nullptr;

        std::shared_ptr<Rendering::Texture2D> perlinTexture = nullptr;
        std::shared_ptr<Rendering::ComputeContext> generatePerlinContext = nullptr;
        std::shared_ptr<Rendering::Material> starMaterial = nullptr;
        std::unique_ptr<UISubEntity> starEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> playerIndicatorTexture = nullptr;
        std::shared_ptr<Rendering::Material> playerIndicatorMaterial = nullptr;
        std::unique_ptr<UISubEntity> playerIndicatorEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> zoomIndicatorTexture = nullptr;
        std::shared_ptr<Rendering::Material> zoomIndicatorMaterial = nullptr;
        std::unique_ptr<UISubEntity> zoomIndicatorEntity = nullptr;

        Function<MouseInput> getMouseInput = nullptr;

    public:
        GalaxyMapEntity(GalaxyMapBuffer* galaxyMapBuffer,
            Function<MouseInput> getMouseInput);

        void Update() override
        {
            manager->Update();

            if (NeedsUpdate())
            {
                rootTransform.HierarchalUpdate();
                GetTreeRenderComp()->UpdateQuadDraw();
                isDirty = false;
                screenVersion = Core::Screen::Version();
                Rendering::Shader::Globals()->SetGalaxyMapRect(rootTransform.Rect);
            }
        }

        GalaxyMapManager* GetMapManager() const { return static_cast<GalaxyMapManager*>(manager.get()); }
        void InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor);

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<GalaxyMapManager>(galaxyMapBuffer,
                &rootTransform,
                getMouseInput,
                playerIndicatorEntity->GetTransform());
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(&rootTransform, galaxyMaterial.get()));
            renderItems.push_back(UIRenderItem(playerIndicatorEntity->GetTransform(), playerIndicatorMaterial.get()));
            renderItems.push_back(UIRenderItem(zoomIndicatorEntity->GetTransform(), zoomIndicatorMaterial.get()));

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
        void InitializePlayerIndicator();
        void InitializeZoomIndicator();
        void UpdateStarPosition(glm::vec2 position);
    };
} // namespace Beer::System
