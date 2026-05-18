#pragma once

#include "General/GalaxyEntity.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/DrawTransparentPass.hpp"
#include "System/Context/IContext.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Stars/StarsEntity.hpp"
#include "System/Light/LightEntity.hpp"
#include "System/Serialization/MapHandler.hpp"
#include <vector>

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    public:
        BeerEvent<void()> OnReturnToPainting;

    private:
        PlayerEntity* playerEntity = nullptr;
        LightEntity* mainLightEntity = nullptr;
        GalaxyEntity* galaxyEntity = nullptr;
        StarsEntity* starsEntity = nullptr;

        Function<PlayerInput> getPlayerInput = nullptr;
        Function<bool> getReturnPressed = nullptr;

        Rendering::DrawOpaquePass* opaquePass = nullptr;
        Rendering::DrawSkyboxPass* skyboxPass = nullptr;
        Rendering::DeferredShadePass* deferredShadePass = nullptr;
        Rendering::DrawTransparentPass* transparentPass = nullptr;

        MapHandler mapHandler;

    public:
        GalaxyContext(Function<PlayerInput> getPlayerInput,
            Function<bool> getReturnPressed,
            MapHandler mapHandler)
            : getPlayerInput(getPlayerInput)
            , getReturnPressed(getReturnPressed)
            , mapHandler(mapHandler)
        {
        }

        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;

    private:
        void InitializePlayer();
        void InitializeLight();
        void InitializeGalaxy();
        void InitializeStars();
        void TryLoadMap();
        void HandleReturn();
    };
} // namespace Beer::System
