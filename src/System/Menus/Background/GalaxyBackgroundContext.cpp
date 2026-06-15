#include "System/Menus/Background/GalaxyBackgroundContext.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Camera/CameraSettings.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

namespace Beer::System
{
    static const glm::vec4 COLOR_A = glm::vec4(0.154f, 0.168f, 0.466f, 1.0f);
    static const glm::vec4 COLOR_B = glm::vec4(0.658f, 0.075f, 0.341f, 1.0f);
    static const glm::vec4 COLOR_C = glm::vec4(0.716f, 0.130f, 0.421f, 1.0f);
    static const uint32_t COLOR_SEED = 18467;

    void GalaxyBackgroundContext::Load()
    {
        InitializeSkybox();
        InitializeCamera();
    }

    void GalaxyBackgroundContext::Update()
    {
        if (backgroundCameraEntity != nullptr)
        {
            backgroundCameraEntity->Update();
        }
    }

    std::vector<Rendering::IRenderPass*> GalaxyBackgroundContext::GetRenderPasses()
    {
        std::vector<Rendering::IRenderPass*> renderPasses;
        renderPasses.push_back(drawSkyboxPass);
        renderPasses.append_range(watercolorSubPipeline->GetRenderPasses());
        return renderPasses;
    }

    void GalaxyBackgroundContext::InitializeSkybox()
    {
        galaxyContainer = std::make_unique<GalaxyContainer>();

        drawSkyboxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawSkyboxPass>(
            std::string(Rendering::SKYBOX_PASS),
            galaxyContainer->GetControlNoiseVolume());

        SerializableGalaxy skyboxGalaxy{};
        skyboxGalaxy.ColorA = COLOR_A;
        skyboxGalaxy.ColorB = COLOR_B;
        skyboxGalaxy.ColorC = COLOR_C;
        skyboxGalaxy.ColorSeed = COLOR_SEED;

        drawSkyboxPass->InitializeNoiseCubemaps(skyboxGalaxy);

        watercolorSubPipeline = std::make_unique<WatercolorSubPipeline>();
    }

    void GalaxyBackgroundContext::InitializeCamera()
    {
        backgroundCameraEntity = registry.CreateEntity<BackgroundCameraEntity>(CAMERA_DEFAULT_SETTINGS);
    }
} // namespace Beer::System
