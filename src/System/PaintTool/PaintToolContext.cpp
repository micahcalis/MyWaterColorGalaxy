#include "System/PaintTool/PaintToolContext.hpp"
#include "BrushSizeBar/BrushSizeBarEntity.hpp"
#include "ColorBar/ColorBarEntity.hpp"
#include "ColorBar/ColorBarLevel.hpp"
#include "ColorBar/ColorBarManager.hpp"
#include "ColorMixer/ColorMixerEntity.hpp"
#include "ColorMixer/PaintSimSubPipeline.hpp"
#include "ColorMixer/PigmentButton.hpp"
#include "GalaxyMap/GalaxyComponent.hpp"
#include "GalaxyMap/GalaxyMapEntity.hpp"
#include "GalaxyMap/GalaxySeed.hpp"
#include "HologramCursor/HologramCursorEntity.hpp"
#include "MenuBar/MenuBarEntity.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"
#include "ToolBar/ToolBarEntity.hpp"
#include <filesystem>
#include <memory>
#include <stdexcept>

namespace Beer::System
{
    void PaintToolContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        InitializeColorPicker();
        InitializeMenuBar();
        InitializeColorBar();
        InitializeGalaxyMap();
        InitializeToolBar();
        InitializeBrushSizeBar();
        InitializeHoloCursor();
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

        if (brushSizeBarEntity != nullptr)
        {
            brushSizeBarEntity->Update();
        }

        if (hologramCursorEntity != nullptr)
        {
            hologramCursorEntity->Update();
        }
    }

    std::vector<Rendering::IRenderPass*> PaintToolContext::GetRenderPasses()
    {
        std::vector<Rendering::IRenderPass*> passes;
        passes.push_back(drawUIPass);

        if (colorMixerEntity != nullptr)
        {
            if (colorMixerEntity->GetEnabled())
                passes.append_range(paintSimSubPipeline->GetRenderPasses());
        }

        return passes;
    }

    void PaintToolContext::InitializeColorPicker()
    {
        colorMixerEntity = registry.CreateEntity<ColorMixerEntity>();

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

        colorMixerEntity->InitializeCloseButton();
        colorMixerEntity->Close();
    }

    void PaintToolContext::InitializeColorBar()
    {
        if (colorMixerEntity == nullptr || menuBarEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Color Bar when Color Mixer or Menu Bar is null!");
        }

        Function<void> openColorPicker = [this]() -> void { colorMixerEntity->Open(); };
        Function<void, glm::vec4> setColorDisplayColor = [this](glm::vec4 color) -> void { colorMixerEntity->SetColorDisplayColor(color); };
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor = [this](glm::vec4 newColor, ColorBarLevel level) -> void { galaxyMapBuffer->SetColorByLevel(newColor, level); };
        Function<std::array<glm::vec4, 4>> getGalaxyColors = [this]() -> std::array<glm::vec4, 4> { return galaxyMapBuffer->GetGalaxyColors(); };

        colorBarEntity = registry.CreateEntity<ColorBarEntity>(getMouseInput,
            openColorPicker,
            setColorDisplayColor,
            setGalaxyBufferColor,
            getGalaxyColors,
            &colorMixerEntity->GetMixerManager()->GetColorPicker()->OnColorPicked,
            &colorMixerEntity->OnColorMixerClosed,
            &menuBarEntity->GetMenuBarManager()->OnNewSeed);

        colorBarEntity->InitializeColorLayers();
    }

    void PaintToolContext::InitializeMenuBar()
    {
        galaxyMapBuffer = std::make_unique<GalaxyMapBuffer>(GalaxySeed());

        menuBarEntity = registry.CreateEntity<MenuBarEntity>(galaxyMapBuffer.get());
        menuBarEntity->InitializeButtonEntities();
    }

    void PaintToolContext::InitializeGalaxyMap()
    {
        if (menuBarEntity == nullptr || galaxyMapBuffer == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Galaxy Map when Menu Bar is null!");
        }

        Function<bool> isColorMixerOpen = [this]() -> bool { return colorMixerEntity->GetEnabled(); };

        galaxyMapEntity = registry.CreateEntity<GalaxyMapEntity>(galaxyMapBuffer.get(), getMouseInput, isColorMixerOpen);

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

        Function<uint32_t, const GalaxyComponentData&> addComponent = [this](const GalaxyComponentData& data) -> uint32_t {
            return galaxyMapEntity->GetMapManager()->GetCursor()->Place(data, true);
        };

        Function<void, uint32_t> eraseComponent = [this](uint32_t index) -> void {
            galaxyMapEntity->GetMapManager()->GetCursor()->Erase(index, true);
        };

        toolBarEntity->InitializeButtons(addComponent, eraseComponent);

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
} // namespace Beer::System
