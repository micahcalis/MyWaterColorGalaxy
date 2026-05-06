#include "System/PaintTool/PaintToolContext.hpp"
#include "ColorBar/ColorBarEntity.hpp"
#include "ColorBar/ColorBarLevel.hpp"
#include "ColorMixer/ColorMixerEntity.hpp"
#include "ColorMixer/PaintSimSubPipeline.hpp"
#include "ColorMixer/PigmentButton.hpp"
#include "GalaxyMap/GalaxyMapEntity.hpp"
#include "GalaxyMap/GalaxySeed.hpp"
#include "MenuBar/MenuBarEntity.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "ToolBar/ToolBarEntity.hpp"
#include <memory>
#include <stdexcept>

namespace Beer::System
{
    void PaintToolContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        InitializeColorPicker();
        InitializeColorBar();
        InitializeMenuBar();
        InitializeGalaxyMap();
        InitializeToolBar();
    }

    void PaintToolContext::Update()
    {
        IContext::Update();

        if (colorMixerEntity != nullptr)
        {
            colorMixerEntity->Update();
        }

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

        if (toolBarEntity != nullptr)
        {
            toolBarEntity->Update();
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
        if (colorMixerEntity == nullptr)
        {
            throw std::runtime_error("Trying to Initialize Color Bar when Color Mixer is null!");
        }

        Function<void> openColorPicker = [this]() -> void { colorMixerEntity->Open(); };
        Function<void, glm::vec4> setColorDisplayColor = [this](glm::vec4 color) -> void { colorMixerEntity->SetColorDisplayColor(color); };

        colorBarEntity = registry.CreateEntity<ColorBarEntity>(getMouseInput,
            openColorPicker,
            setColorDisplayColor,
            &colorMixerEntity->GetMixerManager()->GetColorPicker()->OnColorPicked,
            &colorMixerEntity->OnColorMixerClosed);

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

        galaxyMapEntity = registry.CreateEntity<GalaxyMapEntity>(galaxyMapBuffer.get(), getMouseInput);

        galaxyMapEntity->InitializeCursor(
            [this](ColorBarLevel level)
                -> glm::vec4 { return colorBarEntity->GetColorBarManager()->GetBarColor(level); });
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
    }
} // namespace Beer::System
