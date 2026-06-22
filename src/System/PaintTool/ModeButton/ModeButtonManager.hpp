#pragma once

#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ToolBar/ToolBarManager.hpp"

namespace Beer::System
{
    enum class BrushMenuMode
    {
        ToolMenu,
        ColorMenu
    };

    class ModeButtonManager : public IEntityManager
    {
    private:
        std::unique_ptr<Button> modeToggleButton = nullptr;
        Rendering::Material* buttonMaterial = nullptr;
        Rendering::Texture2D* colorModeTexture = nullptr;
        Rendering::Texture2D* brushModeTexture = nullptr;

        Function<ButtonInput> getTabKeyInput;
        Function<void, bool> setColorModeActive = nullptr;
        Function<void, bool> setBrushModeActive = nullptr;

        BrushMenuMode currentMode = BrushMenuMode::ToolMenu;

        std::shared_ptr<AudioClip> selectClip = nullptr;

    public:
        ModeButtonManager(Function<ButtonInput> getTabKeyInput)
            : getTabKeyInput(getTabKeyInput)
        {
            AudioSettings audioSettings{};
            audioSettings.Volume = SELECT_CLIP_VOLUME;
            selectClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_SelectButton",
                audioSettings);
        }

        void Update() override
        {
            ButtonInput tabInput = getTabKeyInput();

            if (tabInput.ButtonStart)
            {
                ToggleMode();
                selectClip->Play();
            }
        }

        void InitializeModeButton(UITransform* buttonTransform,
            Rendering::Material* buttonMaterial,
            Rendering::Texture2D* colorModeTexture,
            Rendering::Texture2D* brushModeTexture,
            Function<void, bool> setColorModeActive,
            Function<void, bool> setBrushModeActive,
            BrushMenuMode startMode)
        {
            modeToggleButton = std::make_unique<Button>(buttonTransform,
                buttonMaterial);

            this->buttonMaterial = buttonMaterial;
            this->colorModeTexture = colorModeTexture;
            this->brushModeTexture = brushModeTexture;

            modeToggleButton->SetOnClick([this]() -> void {
                ToggleMode();
                selectClip->Play();
            });

            currentMode = startMode;
            setColorModeActive(startMode == BrushMenuMode::ColorMenu);
            setBrushModeActive(startMode == BrushMenuMode::ToolMenu);

            this->setColorModeActive = setColorModeActive;
            this->setBrushModeActive = setBrushModeActive;

            buttonMaterial->SetTexture("_SpriteTex",
                startMode == BrushMenuMode::ColorMenu ? brushModeTexture : colorModeTexture,
                false);
        }

    private:
        void ToggleMode()
        {
            bool isTool = currentMode == BrushMenuMode::ToolMenu;

            if (isTool)
            {
                setColorModeActive(true);
                setBrushModeActive(false);
                buttonMaterial->SetTexture("_SpriteTex", brushModeTexture, false);

            } else
            {
                setColorModeActive(false);
                setBrushModeActive(true);
                buttonMaterial->SetTexture("_SpriteTex", colorModeTexture, false);
            }

            currentMode = isTool ? BrushMenuMode::ColorMenu : BrushMenuMode::ToolMenu;
        }
    };
} // namespace Beer::System
