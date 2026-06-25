#pragma once

#include "Background/BackgroundEntity.hpp"
#include "BrushSizeBar/BrushSizeBarEntity.hpp"
#include "ButtonBlocker/ButtonBlockerEntity.hpp"
#include "HologramCursor/HologramCursorEntity.hpp"
#include "MenuBar/LaunchButtonEntity.hpp"
#include "ModeButton/ModeButtonEntity.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapBuffer.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapEntity.hpp"
#include "System/PaintTool/MenuBar/MenuBarEntity.hpp"
#include "System/PaintTool/ColorMixer/PaintSimSubPipeline.hpp"
#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/PaintTool/ColorMixer/ColorMixerEntity.hpp"
#include "System/PaintTool/ColorBar/ColorBarEntity.hpp"
#include "System/Context/IContext.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Serialization/MapHandler.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include "ToolBar/ToolBarEntity.hpp"
#include "ToolBar/MapBarEntity.hpp"
#include <memory>

namespace Beer::System
{
    class PaintToolContext : public IContext
    {
    public:
        BeerEvent<void()> OnGalaxyFly;
        BeerEvent<void()> OnBackToTitle;

    private:
        std::unique_ptr<GalaxyMapBuffer> galaxyMapBuffer = nullptr;
        ColorMixerEntity* colorMixerEntity = nullptr;
        ColorBarEntity* colorBarEntity = nullptr;
        GalaxyMapEntity* galaxyMapEntity = nullptr;
        MenuBarEntity* menuBarEntity = nullptr;
        LaunchButtonEntity* launchButtonEntity = nullptr;
        ToolBarEntity* toolBarEntity = nullptr;
        MapBarEntity* mapBarEntity = nullptr;
        BrushSizeBarEntity* brushSizeBarEntity = nullptr;
        HologramCursorEntity* hologramCursorEntity = nullptr;
        ModeButtonEntity* modeButtonEntity = nullptr;
        ButtonBlockerEntity* buttonBlockerEntity = nullptr;
        BackgroundEntity* backgroundEntity = nullptr;

        std::unique_ptr<PaintSimSubPipeline> paintSimSubPipeline = nullptr;
        Rendering::DrawUIPass* drawUIPass = nullptr;
        Rendering::FullscreenTransitionPass* transitionPass = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        Function<ButtonInput> getDebugKeyInput = nullptr;
        Function<ButtonInput> getTabKeyInput = nullptr;

        MapHandler mapHandler;
        SerializableGalaxyMap serializedGalaxyMap{};
        Rendering::FadeState initialFadeState;

    public:
        PaintToolContext(Function<MouseInput> getMouseInput,
            Function<ButtonInput> getDebugKeyInput,
            Function<ButtonInput> getTabKeyInput,
            MapHandler mapHandler,
            Rendering::FadeState initialFadeState)
            : getMouseInput(getMouseInput)
            , getDebugKeyInput(getDebugKeyInput)
            , getTabKeyInput(getTabKeyInput)
            , mapHandler(mapHandler)
            , initialFadeState(initialFadeState)
        {
        }

        SerializableGalaxyMap GetSerializedData() const;

        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;

    private:
        void InitializeColorPicker();
        void InitializeColorBar();
        void InitializeMenuBar();
        void InitializeGalaxyMap();
        void InitializeToolBar();
        void InitializeBrushSizeBar();
        void InitializeHoloCursor();
        void InitializeModeButton();
        void InitializeButtonBlocker();
        void InitializeBackground();
        void TryOpenMap();
        SerializablePaintTool SerializePaintTool() const;
    };
} // namespace Beer::System
