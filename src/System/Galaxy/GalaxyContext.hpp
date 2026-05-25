#pragma once

#include "System/Galaxy/General/GalaxyEntity.hpp"
#include "System/Galaxy/General/SunEntity.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/DrawTransparentPass.hpp"
#include "System/Context/IContext.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Stars/StarsEntity.hpp"
#include "System/Galaxy/WatercolorSubPipeline.hpp"
#include "System/Light/LightEntity.hpp"
#include "System/Serialization/MapHandler.hpp"
#include <memory>
#include <vector>

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    public:
        BeerEvent<void()> OnReturnToPainting;
        BeerEvent<void(bool)> OnSetPhotoMode;

    private:
        PlayerEntity* playerEntity = nullptr;
        LightEntity* mainLightEntity = nullptr;
        GalaxyEntity* galaxyEntity = nullptr;
        SunEntity* sunEntity = nullptr;
        StarsEntity* starsEntity = nullptr;

        Function<PlayerInput> getPlayerInput = nullptr;
        Function<bool> getReturnPressed = nullptr;

        std::unique_ptr<WatercolorSubPipeline> watercolorSubPipeline = nullptr;
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
