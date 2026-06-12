#pragma once

#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
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
#include "System/Serialization/SerializableGalaxy.hpp"
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
        GalaxyEntity* galaxyEntity = nullptr;
        SunEntity* sunEntity = nullptr;
        StarsEntity* starsEntity = nullptr;

        Function<PlayerInput> getPlayerInput = nullptr;
        Function<bool> getReturnPressed = nullptr;
        bool isReturning = false;

        std::unique_ptr<WatercolorSubPipeline> watercolorSubPipeline = nullptr;
        Rendering::DrawOpaquePass* opaquePass = nullptr;
        Rendering::DrawSkyboxPass* skyboxPass = nullptr;
        Rendering::DeferredShadePass* deferredShadePass = nullptr;
        Rendering::DrawTransparentPass* transparentPass = nullptr;
        Rendering::FullscreenTransitionPass* transitionPass = nullptr;

        MapHandler mapHandler;
        SerializableGalaxyMap serializedMap{};

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
        void InitializeGalaxy();
        void InitializeStars();
        void InitializeRenderPasses();
        void TryLoadMap();
        void HandleReturn();
        SerializableExplorer SerializeExplorer();
        glm::vec3 GetScaledPlayerPosition() const;
    };
} // namespace Beer::System
