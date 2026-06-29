#pragma once

#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"

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

        std::unique_ptr<UISubEntity> frameEntity = nullptr;
        std::shared_ptr<Rendering::Texture2D> frameTexture = nullptr;
        std::shared_ptr<Rendering::Material> frameMaterial = nullptr;

        std::shared_ptr<Rendering::Texture2D> perlinTexture = nullptr;
        std::shared_ptr<Rendering::ComputeContext> generatePerlinContext = nullptr;
        std::shared_ptr<Rendering::Material> starMaterial = nullptr;
        std::unique_ptr<UISubEntity> starEntity = nullptr;

        std::shared_ptr<Rendering::Texture2D> playerIndicatorTexture = nullptr;
        std::shared_ptr<Rendering::Material> playerIndicatorMaterial = nullptr;
        std::unique_ptr<UISubEntity> playerIndicatorEntity = nullptr;

        std::unique_ptr<HelpButtonSubEntity> helpButtonSubEntity = nullptr;

        Function<MouseInput> getMouseInput = nullptr;

    public:
        GalaxyMapEntity(GalaxyMapBuffer* galaxyMapBuffer,
            Function<MouseInput> getMouseInput);

        void Update() override
        {
            manager->Update();

            if (NeedsUpdate())
            {
                UpdateDirty();
                //  galaxyMapBuffer->PrintComponents();
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
            renderItems.push_back(UIRenderItem(frameEntity->GetTransform(), frameMaterial.get()));
            renderItems.push_back(UIRenderItem(playerIndicatorEntity->GetTransform(), playerIndicatorMaterial.get()));

            if (starEntity != nullptr)
            {
                renderItems.push_back(UIRenderItem(starEntity->GetTransform(), starMaterial.get()));
            }

            renderItems.append_range(galaxyMapBuffer->GetRenderItems());

            if (helpButtonSubEntity != nullptr)
            {
                renderItems.append_range(helpButtonSubEntity->GetRenderItems());
            }

            return renderItems;
        }

    private:
        void InitializeFrame();
        void InitializePerlinWorleyTex();
        void InitializeStar();
        void InitializePerlinTex();
        void InitializePlayerIndicator();
        void InitializeHelpButton();
        void UpdateStarPosition(glm::vec2 position);
    };
} // namespace Beer::System
