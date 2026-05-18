#include "System/Galaxy/GalaxyContext.hpp"
#include "General/GalaxyEntity.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/DrawTransparentPass.hpp"
#include "System/Base/Input/CursorMode.hpp"
#include "System/Context/IContext.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <print>
#include <stdexcept>

namespace Beer::System
{
    static const uint32_t STAR_COUNT = 15'000;
    static const float STAR_BOX_SIZE = 100.0f;

    void GalaxyContext::Load()
    {
        opaquePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawOpaquePass>(
            std::string(Rendering::OPAQUE_PASS));

        skyboxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawSkyboxPass>(
            std::string(Rendering::SKYBOX_PASS));

        deferredShadePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DeferredShadePass>(
            std::string(Rendering::DEFERRED_SHADE_PASS));

        transparentPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawTransparentPass>(
            std::string(Rendering::TRANSPARENT_PASS));

        InitializeLight();
        InitializePlayer();
        InitializeGalaxy();
        InitializeStars();
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
        return {opaquePass, deferredShadePass, skyboxPass, transparentPass};
    }

    void GalaxyContext::InitializePlayer()
    {
        playerEntity = registry.CreateEntity<PlayerEntity>(getPlayerInput);
    }

    void GalaxyContext::InitializeLight()
    {
        Transform lightTransform{};
        lightTransform.Position = glm::vec3(0, 1000, 100);

        mainLightEntity = registry.CreateEntity<LightEntity>(std::move(lightTransform),
            10,
            glm::vec4(1, 1, 0.8, 1),
            glm::vec4(0.2, 0.23, 0.35, 1),
            glm::vec4(0.86, 0.98, 1, 1));
    }

    void GalaxyContext::InitializeGalaxy()
    {
        galaxyEntity = registry.CreateEntity<GalaxyEntity>();
    }

    void GalaxyContext::InitializeStars()
    {
        starsEntity = registry.CreateEntity<StarsEntity>(STAR_COUNT, STAR_BOX_SIZE);
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

        SerializablePaintSession serializedPaintSession = mapHandler.Load();
        galaxyEntity->LoadFromSerialized(serializedPaintSession.Galaxy);
    }

    void GalaxyContext::HandleReturn()
    {
        bool returnPressed = getReturnPressed();

        if (returnPressed)
        {
            OnReturnToPainting.Invoke();
        }
    }
} // namespace Beer::System
