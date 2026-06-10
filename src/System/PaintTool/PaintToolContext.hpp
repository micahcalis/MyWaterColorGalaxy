#pragma once

#include "BrushSizeBar/BrushSizeBarEntity.hpp"
#include "HologramCursor/HologramCursorEntity.hpp"
#include "ModeButton/ModeButtonEntity.hpp"
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
#include <memory>

namespace Beer::System
{
    class PaintToolContext : public IContext
    {
    public:
        BeerEvent<void()> OnGalaxyFly;

    private:
        std::unique_ptr<GalaxyMapBuffer> galaxyMapBuffer = nullptr;
        ColorMixerEntity* colorMixerEntity = nullptr;
        ColorBarEntity* colorBarEntity = nullptr;
        GalaxyMapEntity* galaxyMapEntity = nullptr;
        MenuBarEntity* menuBarEntity = nullptr;
        ToolBarEntity* toolBarEntity = nullptr;
        BrushSizeBarEntity* brushSizeBarEntity = nullptr;
        HologramCursorEntity* hologramCursorEntity = nullptr;
        ModeButtonEntity* modeButtonEntity = nullptr;
        std::unique_ptr<PaintSimSubPipeline> paintSimSubPipeline = nullptr;
        Rendering::DrawUIPass* drawUIPass = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        Function<ButtonInput> getDebugKeyInput = nullptr;
        Function<ButtonInput> getTabKeyInput = nullptr;
        bool toggle = false;
        MapHandler mapHandler;
        SerializableGalaxyMap serializedGalaxyMap{};

    public:
        PaintToolContext(Function<MouseInput> getMouseInput,
            Function<ButtonInput> getDebugKeyInput,
            Function<ButtonInput> getTabKeyInput,
            MapHandler mapHandler)
            : getMouseInput(getMouseInput)
            , getDebugKeyInput(getDebugKeyInput)
            , getTabKeyInput(getTabKeyInput)
            , mapHandler(mapHandler)
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
        void TryOpenMap();
        SerializablePaintTool SerializePaintTool() const;
    };
} // namespace Beer::System
