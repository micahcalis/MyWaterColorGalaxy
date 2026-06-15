#include "System/Galaxy/GalaxyContext.hpp"
#include "General/Buffer/GalaxyObjectBuffer.hpp"
#include "General/GalaxyEntity.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/DrawTransparentPass.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "System/Base/Clock/Clock.hpp"
#include "System/Base/Input/CursorMode.hpp"
#include "System/Context/IContext.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <print>
#include <stdexcept>

namespace Beer::System
{
    static const uint32_t STAR_COUNT = 15'000;
    static const float STAR_BOX_SIZE = 100.0f;
    static const float FADE_DURATION = 1.0f;

    void GalaxyContext::Load()
    {
        InitializeGalaxy();
        InitializePlayer();
        InitializeStars();
        InitializeRenderPasses();
        TryLoadMap();

        Cursor::SetCursorMode(CursorMode::Locked);
    }

    void GalaxyContext::Update()
    {
        IContext::Update();

        HandleReturn();

        if (playerEntity != nullptr)
        {
            playerEntity->Update();
        }

        if (galaxyEntity != nullptr)
        {
            galaxyEntity->Update();
        }
    }

    std::vector<Rendering::IRenderPass*> GalaxyContext::GetRenderPasses()
    {
        std::vector<Rendering::IRenderPass*> passes;
        passes.reserve(20);

        passes.append_range(watercolorSubPipeline->GetRenderPasses());
        passes.push_back(opaquePass);
        passes.push_back(deferredShadePass);
        passes.push_back(skyboxPass);
        passes.push_back(transparentPass);
        passes.push_back(transitionPass);

        return passes;
    }

    void GalaxyContext::FadeReturn()
    {
        if (isReturning == false)
        {
            Function<void> onFadeIn = [this]() -> void {
                serializedMap.ExplorerHistory = SerializeExplorer();
                mapHandler.Save(serializedMap);
                OnReturnToPainting.Invoke();
            };

            transitionPass->SetFade(Rendering::FadeState::In, 1.0f / FADE_DURATION);

            auto returnTimer = Clock::Timer(FADE_DURATION);
            returnTimer->OnTimerComplete.Subscribe(onFadeIn);
            returnTimer->Start();

            isReturning = true;
        }
    }

    void GalaxyContext::TogglePhotoMode()
    {
        playerEntity->GetPlayerManager()->TogglePhotoMode();
    }

    void GalaxyContext::InitializePlayer()
    {
        if (galaxyEntity == nullptr)
        {
            throw std::runtime_error("Trying To Initialize Player when Galaxy is null!");
        }

        playerEntity = registry.CreateEntity<PlayerEntity>(getPlayerInput,
            &OnSetPhotoMode,
            galaxyEntity->GetContainer()->GetControlNoiseVolume());
    }

    void GalaxyContext::InitializeGalaxy()
    {
        galaxyEntity = registry.CreateEntity<GalaxyEntity>();
        sunEntity = registry.CreateEntity<SunEntity>(galaxyEntity->GetContainer()->GetControlNoiseVolume());
    }

    void GalaxyContext::InitializeStars()
    {
        if (galaxyEntity == nullptr)
        {
            throw std::runtime_error("Trying To Initialize Stars when Galaxy is null!");
        }

        starsEntity = registry.CreateEntity<StarsEntity>(STAR_COUNT, STAR_BOX_SIZE);
    }

    void GalaxyContext::InitializeRenderPasses()
    {
        if (galaxyEntity == nullptr)
        {
            throw std::runtime_error("Trying To Initialize Render Passes when Galaxy is null!");
        }

        opaquePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawOpaquePass>(
            Rendering::OPAQUE_PASS);

        skyboxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawSkyboxPass>(
            Rendering::SKYBOX_PASS,
            galaxyEntity->GetContainer()->GetControlNoiseVolume());

        deferredShadePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DeferredShadePass>(
            Rendering::DEFERRED_SHADE_PASS);

        transparentPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawTransparentPass>(
            Rendering::TRANSPARENT_PASS);

        transitionPass = Rendering::IRenderPass::FetchFromRegister<Rendering::FullscreenTransitionPass>(
            Rendering::TRANSITION_PASS);

        watercolorSubPipeline = std::make_unique<WatercolorSubPipeline>();

        if (!transitionPass->HasMaterial())
        {
            auto transitionMaterial = std::make_shared<Rendering::Material>("Blit/SpaceTransitionBlit");
            transitionPass->SetMaterial(transitionMaterial);
        }

        transitionPass->SetFade(Rendering::FadeState::Out, 1.0f / FADE_DURATION);
    }

    void GalaxyContext::TryLoadMap()
    {
        if (galaxyEntity == nullptr)
        {
            throw std::runtime_error("Trying To Load Map when Galaxy Entity is null!");
        }

        if (!mapHandler.IsSaved())
        {
            throw std::runtime_error("Trying to Load Map that doesn't exist!");
        }

        serializedMap = mapHandler.Load();
        galaxyEntity->LoadFromSerialized(serializedMap.Galaxy);
        sunEntity->LoadFromSerialized(serializedMap.Galaxy);
        skyboxPass->InitializeNoiseCubemaps(serializedMap.Galaxy);
        playerEntity->GetPlayerManager()->LoadFromSerialized(serializedMap.ExplorerHistory);
    }

    void GalaxyContext::HandleReturn()
    {
        bool returnPressed = getReturnPressed();

        if (returnPressed)
        {
            FadeReturn();
        }
    }

    SerializableExplorer GalaxyContext::SerializeExplorer()
    {
        SerializableExplorer serializedExplorer{};
        serializedExplorer.PlayerPosition = GetScaledPlayerPosition();

        glm::quat playerRot = playerEntity->GetTransform()->Rotation;
        serializedExplorer.PlayerRotation = glm::vec4(playerRot.w, playerRot.x, playerRot.y, playerRot.z);

        return serializedExplorer;
    }

    glm::vec3 GalaxyContext::GetScaledPlayerPosition() const
    {
        glm::vec3 scaledPos = (playerEntity->GetTransform()->Position) / GalaxyObjectBuffer::GALAXY_POS_SCALE;
        return glm::clamp(scaledPos, glm::vec3(0.0f), glm::vec3(1.0f));
    }
} // namespace Beer::System
