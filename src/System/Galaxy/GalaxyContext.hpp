#pragma once

#include "General/GalaxyEntity.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/ComputeTornadoParticlesPass.hpp"
#include "Rendering/RenderPasses/DeferredShadePass.hpp"
#include "Rendering/RenderPasses/DrawOpaquePass.hpp"
#include "Rendering/RenderPasses/DrawSkyboxPass.hpp"
#include "Rendering/RenderPasses/ComputePerlinPass.hpp"
#include "Rendering/RenderPasses/DrawTransparentPass.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "Rendering/RenderPasses/RenderTornadoPass.hpp"
#include "Rendering/Text/FontAsset.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Context/IContext.hpp"
#include "System/Default/MultipleContainerEntity.hpp"
#include "System/Default/RotateEntitiesManager.hpp"
#include "System/Default/SingleStaticEntity.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Galaxy/Stars/StarsEntity.hpp"
#include "System/Light/LightEntity.hpp"
#include "System/Default/UI/TextDisplayEntity.hpp"
#include "System/Default/UI/TestQuadTreeEntity.hpp"
#include "System/Serialization/MapHandler.hpp"
#include <vector>

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    private:
        PlayerEntity* playerEntity = nullptr;
        LightEntity* mainLightEntity = nullptr;
        GalaxyEntity* galaxyEntity = nullptr;
        StarsEntity* starsEntity = nullptr;

        Function<PlayerInput> getPlayerInput;

        Rendering::DrawOpaquePass* opaquePass = nullptr;
        Rendering::DrawSkyboxPass* skyboxPass = nullptr;
        Rendering::DeferredShadePass* deferredShadePass = nullptr;
        Rendering::DrawTransparentPass* transparentPass = nullptr;

        MapHandler mapHandler;

    public:
        GalaxyContext(Function<PlayerInput> getPlayerInput,
            MapHandler mapHandler)
            : getPlayerInput(getPlayerInput)
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
    };
} // namespace Beer::System
