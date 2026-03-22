#pragma once

#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "System/Context/IContext.hpp"
#include "System/Default/SingleStaticEntity.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Light/LightEntity.hpp"
#include <vector>

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    private:
        PlayerEntity* playerEntity;
        LightEntity* mainLightEntity;
        std::vector<SingleStaticEntity*> staticEntities;
        Function<PlayerInput> getPlayerInput;

        Rendering::DrawOpaquePass* opaquePass = nullptr;
        Rendering::DrawSkyboxPass* skyboxPass = nullptr;

    public:
        GalaxyContext(Function<PlayerInput> getPlayerInput);
        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;
    };
} // namespace Beer::System
