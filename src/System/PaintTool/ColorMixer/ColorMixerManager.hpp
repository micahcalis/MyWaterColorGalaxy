#pragma once

#include "ColorMixerCursor.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Audio/SoundGlobalSettings.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/PaintTool/ColorMixer/PigmentButton.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/HelpToggle/HelpContainer.hpp"
#include "System/PaintTool/HelpToggle/HelpToggle.hpp"
#include <vector>
#include "System/PaintTool/ToolBar/ToolBarManager.hpp"

namespace Beer::System
{
    class ColorMixerManager : public IEntityManager
        , public HelpContainer
    {
    public:
        BeerEvent<void(PigmentType)> OnPigmentClicked;

    private:
        UITransform* canvasTransform = nullptr;
        Function<void> markDirty = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        std::vector<std::unique_ptr<PigmentButton>> pigmentButtons;
        PigmentType currentPigment = PigmentType::QuinacridoneRose;
        std::unique_ptr<Button> clearButton = nullptr;
        Function<void> clearColorMixer = nullptr;
        std::unique_ptr<ColorPicker> colorPicker = nullptr;
        std::unique_ptr<ColorMixerCursor> colorMixerCursor = nullptr;
        UITransform* selectSpriteTransform = nullptr;
        bool enabled = false;
        std::shared_ptr<AudioClip> selectClip = nullptr;
        std::shared_ptr<AudioClip> clearCanvasClip = nullptr;

    public:
        ColorMixerManager(UITransform* canvasTransform,
            Function<void> markDirty,
            Function<MouseInput> getMouseInput)
            : canvasTransform(canvasTransform)
            , markDirty(markDirty)
            , getMouseInput(getMouseInput)
        {
            AudioSettings audioSettings{};
            audioSettings.Volume = SELECT_CLIP_VOLUME;
            selectClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_SelectButton",
                audioSettings);

            audioSettings.Volume = CLEAR_CANVAS_VOLUME;
            clearCanvasClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_ClearCanvas",
                audioSettings);
        }

        void Update() override;

        void AddPigmentButton(UITransform* transform,
            Rendering::Material* spriteMaterial,
            PigmentType pigment);

        void SetCurrentPigment(PigmentType pigment, UITransform* transform);
        PigmentType GetCurrentPigment() const { return currentPigment; }

        void SetCurrentPigmentByIndex(uint32_t index);

        void SetClearButton(Function<void> clearColorMixer,
            UITransform* clearTransform,
            Rendering::Material* clearMaterial);

        void SetColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
            Function<MouseInput> getMouseInput,
            UITransform* canvasTransform);

        void SetColorMixerCursor(UITransform* canvasTransform,
            UITransform* cursorTransform,
            Rendering::Material* cursorMaterial,
            Rendering::Texture2D* brushTexture,
            Rendering::Texture2D* brushMask,
            Rendering::Texture2D* pickerTexture,
            Rendering::Texture2D* pickerMask);

        void SetSelectButton(UITransform* selectTransform)
        {
            selectSpriteTransform = selectTransform;
        }

        bool ClearButtonInitialized() const
        {
            return clearButton != nullptr;
        }

        bool ColorPickerInitialized() const
        {
            return colorPicker != nullptr;
        }

        [[nodiscard]] ColorPicker* GetColorPicker() const
        {
            if (colorPicker == nullptr)
                return nullptr;

            return colorPicker.get();
        }

        [[nodiscard]] ColorMixerCursor* GetMixerCursor() const
        {
            if (colorMixerCursor == nullptr)
                return nullptr;

            return colorMixerCursor.get();
        }

        void DeselectPigments()
        {
            UITransform* selectParent = selectSpriteTransform->Parent;

            if (selectParent == nullptr)
            {
                return;
            }

            selectParent->UnbindChild(selectSpriteTransform);
            markDirty();
        }

        void SetEnabled(bool enabled)
        {
            this->enabled = enabled;
        }

    private:
        void ClearColorMixer();
        void CloseColorMixer();
    };
} // namespace Beer::System
