#pragma once

#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "System/Menus/Background/BackgroundCameraEntity.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "System/Context/IContext.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"

namespace Beer::System
{
    class GalaxyBackgroundContext : public IContext
    {
    private:
        Rendering::DrawSkyboxPass* drawSkyboxPass = nullptr;
        std::unique_ptr<GalaxyContainer> galaxyContainer = nullptr;
        BackgroundCameraEntity* backgroundCameraEntity = nullptr;
        std::unique_ptr<WatercolorSubPipeline> watercolorSubPipeline;

    public:
        GalaxyBackgroundContext()
        {
        }

        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;

    private:
        void InitializeSkybox();
        void InitializeCamera();
    };
} // namespace Beer::System
