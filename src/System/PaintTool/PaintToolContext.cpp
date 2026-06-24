#include "System/PaintTool/PaintToolContext.hpp"
#include "BrushSizeBar/BrushSizeBarEntity.hpp"
#include "ButtonBlocker/ButtonBlockerEntity.hpp"
#include "ColorBar/ColorBarEntity.hpp"
#include "ColorBar/ColorBarLevel.hpp"
#include "ColorBar/ColorBarManager.hpp"
#include "ColorMixer/ColorMixerCursor.hpp"
#include "ColorMixer/ColorMixerEntity.hpp"
#include "ColorMixer/PaintSimSubPipeline.hpp"
#include "ColorMixer/PigmentButton.hpp"
#include "GalaxyMap/GalaxyComponent.hpp"
#include "GalaxyMap/GalaxyMapEntity.hpp"
#include "GalaxyMap/GalaxySeed.hpp"
#include "HologramCursor/HologramCursorEntity.hpp"
#include "MenuBar/LaunchButtonEntity.hpp"
#include "MenuBar/MenuBarEntity.hpp"
#include "ModeButton/ModeButtonEntity.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Base/Input/CursorMode.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include "ToolBar/MapBarEntity.hpp"
#include "ToolBar/ToolBarEntity.hpp"
#include "TransitionMaterialGetter.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <memory>
#include <print>
#include <stdexcept>

namespace Beer::System
{
    void PaintToolContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        transitionPass = Rendering::IRenderPass::FetchFromRegister<Rendering::FullscreenTransitionPass>(
            std::string(Rendering::TRANSITION_PASS));

        if (!transitionPass->HasMaterial())
        {
            auto transitionInitializationFunc = TransitionMaterialGetter::GetHyperspaceInitialization();
            transitionPass->Initialize(transitionInitializationFunc);
        }

        InitializeColorPicker();
        InitializeGalaxyMap();
        InitializeToolBar();
        InitializeButtonBlocker();
        InitializeMenuBar();
        InitializeColorBar();
        InitializeBrushSizeBar();
        InitializeHoloCursor();
        InitializeModeButton();

        TryOpenMap();

        Cursor::SetCursorMode(CursorMode::Unlocked);
    }

    SerializableGalaxyMap PaintToolContext::GetSerializedData() const
    {
        SerializableGalaxyMap serializedData{};
        serializedData.Galaxy = galaxyMapBuffer->GetSerializableGalaxy();
        serializedData.ToolHistory = SerializePaintTool();
        serializedData.ExplorerHistory = serializedGalaxyMap.ExplorerHistory;
        return serializedData;
    }

    void PaintToolContext::Update()
    {
        IContext::Update();

        if (colorBarEntity != nullptr)
        {
            colorBarEntity->Update();
        }

        if (menuBarEntity != nullptr)
        {
            menuBarEntity->Update();
        }

        if (launchButtonEntity != nullptr)
        {
            launchButtonEntity->Update();
        }

        if (galaxyMapEntity != nullptr)
        {
            galaxyMapEntity->Update();
        }

        if (colorMixerEntity != nullptr)
        {
            colorMixerEntity->Update();
        }

        if (toolBarEntity != nullptr)
        {
            toolBarEntity->Update();
        }

        if (mapBarEntity != nullptr)
        {
            mapBarEntity->Update();
        }

        if (brushSizeBarEntity != nullptr)
        {
            brushSizeBarEntity->Update();
        }

        if (hologramCursorEntity != nullptr)
        {
            hologramCursorEntity->Update();
        }

        if (modeButtonEntity != nullptr)
        {
            modeButtonEntity->Update();
        }

        if (buttonBlockerEntity != nullptr)
        {
            buttonBlockerEntity->Update();
        }
    }

    std::vector<Rendering::IRenderPass*> PaintToolContext::GetRenderPasses()
    {
        std::vector<Rendering::IRenderPass*> passes;
        passes.push_back(drawUIPass);
        passes.push_back(transitionPass);

        if (colorMixerEntity != nullptr)
        {
            if (colorMixerEntity->GetEnabled())
                passes.append_range(paintSimSubPipeline->GetRenderPasses());
        }

        return passes;
    }

    void PaintToolContext::InitializeColorPicker()
    {
        colorMixerEntity = registry.CreateEntity<ColorMixerEntity>(getMouseInput);

        paintSimSubPipeline = std::make_unique<PaintSimSubPipeline>(
            colorMixerEntity->GetColorMixerMat(),
            getMouseInput,
            [this]() -> UITransform* { return colorMixerEntity->GetRootTransform(); },
            getDebugKeyInput);

        paintSimSubPipeline->GetInjectPaintPass()->SetGetCurrentPigment([this]() -> PigmentType {
            return colorMixerEntity->GetMixerManager()->GetCurrentPigment();
        });

        colorMixerEntity->InitializeClearButton([this]() -> void {
            paintSimSubPipeline->MarkClear();
        });

        Function<void, Function<void, ImagePixelData>> colorPickReadbackFunc =
            [this](Function<void, ImagePixelData> readbackFunc) -> void { paintSimSubPipeline->SubscribeToNewCanvasReadback(readbackFunc); };

        colorMixerEntity->InitializeColorPicker(colorPickReadbackFunc,
            getMouseInput);

        paintSimSubPipeline->SetGetColorPickerState([this]() -> ColorPickingState {
            return colorMixerEntity->GetMixerManager()->GetColorPicker()->GetState();
        });

        colorMixerEntity->GetMixerManager()->SetCurrentPigmentByIndex(0);
    }

    void PaintToolContext::InitializeColorBar()
    {
        if (colorMixerEntity == nullptr || menuBarEntity == nullptr || toolBarEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Color Bar when Color Mixer or Menu Bar or Tool Bar is null!");
        }

        Function<void, glm::vec4> setColorDisplayColor = [this](glm::vec4 color) -> void { colorMixerEntity->SetColorDisplayColor(color); };
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor = [this](glm::vec4 newColor, ColorBarLevel level) -> void { galaxyMapBuffer->SetColorByLevel(newColor, level); };
        Function<std::array<glm::vec4, 4>> getGalaxyColors = [this]() -> std::array<glm::vec4, 4> { return galaxyMapBuffer->GetGalaxyColors(); };
        Function<void> pickerSelectColor = [this]() -> void { colorMixerEntity->GetMixerManager()->GetColorPicker()->SelectColor(); };

        Function<Rendering::Texture2D*> getBrushTexture =
            [this]() -> Rendering::Texture2D* {
            GalaxyBrushType brushType = galaxyMapEntity->GetMapManager()->GetCursor()->Brush;

            if (!IsGalaxyComponent(brushType))
            {
                brushType = GalaxyBrushType::Planet;
            }

            return galaxyMapEntity->GetMapManager()->GetCursor()->GetFactory()->GetTexture(brushType);
        };

        colorBarEntity = registry.CreateEntity<ColorBarEntity>(getMouseInput,
            setColorDisplayColor,
            setGalaxyBufferColor,
            getGalaxyColors,
            getBrushTexture,
            pickerSelectColor,
            &colorMixerEntity->GetMixerManager()->GetColorPicker()->OnColorPicked,
            &colorMixerEntity->OnColorMixerClosed,
            &menuBarEntity->GetMenuBarManager()->OnNewSeed);

        Function<void, glm::vec4, ColorBarType, ColorBarLevel> setDisplaysColor = [this](glm::vec4 color, ColorBarType type, ColorBarLevel level) -> void {
            if (type == ColorBarType::Planet)
            {
                colorMixerEntity->GetMixerManager()->GetColorDisplayHandler()->SetColor(color, level);
                toolBarEntity->GetToolBarManager()->GetColorDisplayHandler()->SetColor(color, level);
            }
        };

        colorBarEntity->GetColorBarManager()->OnBarColorChanged.Subscribe(setDisplaysColor);

        colorBarEntity->InitializeColorLayers();

        galaxyMapEntity->GetMapManager()->OnNewBrush.Subscribe([this](GalaxyBrushType type) -> void {
            colorBarEntity->GetColorBarManager()->UpdateDisplayMaterials();
        });

        Function<void, PigmentType> deselectColorBar = [this](PigmentType pigment) -> void { colorBarEntity->GetColorBarManager()->DeselectColors(); };
        Function<void, glm::vec4> deselectPigments = [this](glm::vec4 color) -> void { colorMixerEntity->GetMixerManager()->DeselectPigments(); };

        colorMixerEntity->GetMixerManager()->OnPigmentClicked.Subscribe(deselectColorBar);
        colorBarEntity->GetColorBarManager()->OnColorClicked.Subscribe(deselectPigments);

        Function<void, glm::vec4> setMixerCursor = [this](glm::vec4 color) -> void {
            colorMixerEntity->GetMixerManager()->GetMixerCursor()->SetCursor(MixerCursorType::Picker, color);
        };

        colorBarEntity->GetColorBarManager()->OnColorClicked.Subscribe(setMixerCursor);
    }

    void PaintToolContext::InitializeMenuBar()
    {
        if (galaxyMapEntity == nullptr || toolBarEntity == nullptr || buttonBlockerEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Menu Bar when Galaxy Map or Tool Bar Entity or Button Blocker is null!");
        }

        Function<void> clearHistory = [this]() -> void { toolBarEntity->GetToolBarManager()->GetHistoryController()->ClearHistory(); };

        Function<void> saveMap = [this]() -> void {
            SerializableGalaxyMap serializedGalaxyMap = GetSerializedData();
            mapHandler.Save(serializedGalaxyMap);
            OnGalaxyFly.Invoke();
        };

        Function<void> onBackToTitle = [this]() -> void {
            SerializableGalaxyMap serializedGalaxyMap = GetSerializedData();
            mapHandler.Save(serializedGalaxyMap);
            OnBackToTitle.Invoke();
        };

        Function<void> enableBlock = [this]() -> void {
            buttonBlockerEntity->SetTreeEnabled(true);
        };

        menuBarEntity = registry.CreateEntity<MenuBarEntity>(galaxyMapBuffer.get(),
            clearHistory,
            saveMap,
            onBackToTitle,
            enableBlock,
            initialFadeState);

        launchButtonEntity = registry.CreateEntity<LaunchButtonEntity>();

        menuBarEntity->InitializeButtons(launchButtonEntity->GetButtonTransform(), launchButtonEntity->GetButtonMaterial());
    }

    void PaintToolContext::InitializeGalaxyMap()
    {
        if (colorMixerEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Galaxy Map when Color Mixer is null!");
        }

        galaxyMapBuffer = std::make_unique<GalaxyMapBuffer>(GalaxySeed());

        galaxyMapEntity = registry.CreateEntity<GalaxyMapEntity>(galaxyMapBuffer.get(), getMouseInput);

        galaxyMapEntity->InitializeCursor(
            [this](ColorBarLevel level)
                -> glm::vec4 { return colorBarEntity->GetColorBarManager()->GetBarColor(level, ColorBarType::Planet); });
    }

    void PaintToolContext::InitializeToolBar()
    {
        if (galaxyMapEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Tool Bar when Galaxy Map is null!");
        }

        Function<void, GalaxyBrushType> setBrushType = [this](GalaxyBrushType brushType) -> void {
            galaxyMapEntity->GetMapManager()->SetBrushType(brushType);
        };

        toolBarEntity = registry.CreateEntity<ToolBarEntity>(setBrushType);

        toolBarEntity->InitializeButtons();

        Function<uint32_t, const GalaxyComponentData&> addComponent = [this](const GalaxyComponentData& data) -> uint32_t {
            return galaxyMapEntity->GetMapManager()->GetCursor()->Place(data, true);
        };

        Function<void, uint32_t> eraseComponent = [this](uint32_t index) -> void {
            galaxyMapEntity->GetMapManager()->GetCursor()->Erase(index, true);
        };

        MapBarInitializationFunc initializeTools = [this](std::vector<std::unique_ptr<UISubEntity>>& entities,
                                                       std::vector<std::shared_ptr<Rendering::Material>>& materials) {
            toolBarEntity->InitializeTools(MapBarEntity::TOOL_TYPES,
                entities,
                materials);
        };

        MapBarInitializationFunc initializeHistory
            = [this, addComponent, eraseComponent](std::vector<std::unique_ptr<UISubEntity>>& entities, std::vector<std::shared_ptr<Rendering::Material>>& materials) -> void {
            toolBarEntity->InitializeHistoryButtons(addComponent,
                eraseComponent,
                entities,
                materials);
        };

        mapBarEntity = registry.CreateEntity<MapBarEntity>();
        mapBarEntity->InitializeButtons(galaxyMapEntity->GetRootTransform(),
            initializeTools,
            initializeHistory);

        toolBarEntity->BindHistoryActions(
            &galaxyMapEntity->GetMapManager()->GetCursor()->OnComponentPlaced,
            &galaxyMapEntity->GetMapManager()->GetCursor()->OnComponentErased);
    }

    void PaintToolContext::InitializeBrushSizeBar()
    {
        if (galaxyMapEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Brush Size Bar when Galaxy Map is null!");
        }

        Function<void, float> setBrushSize = [this](float size) -> void {
            galaxyMapEntity->GetMapManager()->GetCursor()->SetSize(size);
        };

        brushSizeBarEntity = registry.CreateEntity<BrushSizeBarEntity>(setBrushSize);
        brushSizeBarEntity->InitializeSlider();
    }

    void PaintToolContext::InitializeHoloCursor()
    {
        if (galaxyMapEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Holo Cursor when Galaxy Map is null!");
        }

        Function<CursorState> getCursorState = [this]() -> CursorState {
            return galaxyMapEntity->GetMapManager()->GetCursorState();
        };

        Function<Rendering::Texture2D*, GalaxyBrushType> getBrushTexture =
            [this](GalaxyBrushType type) -> Rendering::Texture2D* {
            return galaxyMapEntity->GetMapManager()->GetCursor()->GetFactory()->GetTexture(type);
        };

        hologramCursorEntity = registry.CreateEntity<HologramCursorEntity>(getCursorState,
            getMouseInput,
            getBrushTexture);
    }

    void PaintToolContext::InitializeModeButton()
    {
        if (colorMixerEntity == nullptr)
        {
            throw std::runtime_error("Trying to Mode Button when Color Mixer is null!");
        }

        if (brushSizeBarEntity == nullptr)
        {
            throw std::runtime_error("Trying to Mode Button when Brush Size Bar is null!");
        }

        if (toolBarEntity == nullptr)
        {
            throw std::runtime_error("Trying to Mode Button when Tool Bar is null!");
        }

        Function<void, bool> setColorModeActive = [this](bool enabled) -> void {
            colorBarEntity->SetTreeEnabled(enabled);
            colorMixerEntity->SetTreeEnabled(enabled);
            colorMixerEntity->GetMixerManager()->SetEnabled(enabled);
        };

        Function<void, bool> setBrushModeActive = [this](bool enabled) -> void {
            toolBarEntity->SetTreeEnabled(enabled);
            brushSizeBarEntity->SetTreeEnabled(enabled); };

        modeButtonEntity = registry.CreateEntity<ModeButtonEntity>(setColorModeActive,
            setBrushModeActive,
            getTabKeyInput);

        modeButtonEntity->InitializeModeButton();
    }

    void PaintToolContext::InitializeButtonBlocker()
    {
        buttonBlockerEntity = registry.CreateEntity<ButtonBlockerEntity>(ContextType::PaintTool);
        buttonBlockerEntity->InitializeBlocker();
        buttonBlockerEntity->SetTreeEnabled(false);
    }

    void PaintToolContext::TryOpenMap()
    {
        if (!mapHandler.IsSaved())
            return;

        serializedGalaxyMap = mapHandler.Load();
        galaxyMapEntity->GetMapManager()->ReloadFromSerialized(serializedGalaxyMap);
        colorBarEntity->GetColorBarManager()->ReloadFromSerialized(serializedGalaxyMap.ToolHistory);
        toolBarEntity->GetToolBarManager()->GetHistoryController()->ClearHistory();
        toolBarEntity->GetToolBarManager()->SetCurrentBrush(static_cast<GalaxyBrushType>(serializedGalaxyMap.ToolHistory.SelectedType));

        float normalizedSize = galaxyMapEntity->GetMapManager()->GetCursor()->GetNormalizedSize();
        brushSizeBarEntity->GetBrushSizeBarManager()->GetSlider()->ForceUpdate(normalizedSize);
    }

    SerializablePaintTool PaintToolContext::SerializePaintTool() const
    {
        SerializablePaintTool serializedTool{};
        serializedTool.ComponentColors = colorBarEntity->GetColorBarManager()->GetColors(ColorBarType::Planet);
        serializedTool.GalaxyColors = colorBarEntity->GetColorBarManager()->GetColors(ColorBarType::Galaxy);

        serializedTool.BrushSize = galaxyMapEntity->GetMapManager()->GetCursor()->Size;
        serializedTool.SelectedType = static_cast<uint32_t>(galaxyMapEntity->GetMapManager()->GetCursor()->Brush);

        serializedTool.ZoomScale = galaxyMapEntity->GetMapManager()->GetZoomer()->Zoom;
        serializedTool.ZoomPanning = galaxyMapEntity->GetMapManager()->GetZoomer()->Panning;

        return serializedTool;
    }
} // namespace Beer::System
