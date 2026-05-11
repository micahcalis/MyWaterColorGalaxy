#pragma once

#include "System/Galaxy/GalaxyContext.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "System/Context/IContext.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"

namespace Beer::System
{
    GalaxyContext::GalaxyContext(Function<PlayerInput> getPlayerInput)
    {
        this->getPlayerInput = getPlayerInput;

        opaquePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawOpaquePass>(
            std::string(Rendering::OPAQUE_PASS));

        skyboxPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawSkyboxPass>(
            std::string(Rendering::SKYBOX_PASS));

        deferredShadePass = Rendering::IRenderPass::FetchFromRegister<Rendering::DeferredShadePass>(
            std::string(Rendering::DEFERRED_SHADE_PASS));
    }

    void GalaxyContext::Update()
    {
        IContext::Update();

        if (playerEntity != nullptr)
        {
            playerEntity->Update();
        }
    }

    void GalaxyContext::Load()
    {
        Transform lightTransform{};
        lightTransform.Position = glm::vec3(0, 1000, 100);

        mainLightEntity = registry.CreateEntity<LightEntity>(std::move(lightTransform),
            10,
            glm::vec4(1, 1, 0.8, 1),
            glm::vec4(0.2, 0.23, 0.35, 1),
            glm::vec4(0.86, 0.98, 1, 1));

        Transform playerTransform{};
        playerTransform.Position = PLAYER_SETTINGS.StartPos;

        playerEntity = registry.CreateEntity<PlayerEntity>(std::move(playerTransform),
            nullptr,
            getPlayerInput);
    }

    std::vector<Rendering::IRenderPass*> GalaxyContext::GetRenderPasses()
    {
        return {opaquePass, deferredShadePass, skyboxPass};
    }
} // namespace Beer::System
