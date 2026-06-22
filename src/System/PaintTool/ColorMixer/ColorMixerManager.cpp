#include "System/PaintTool/ColorMixer/ColorMixerManager.hpp"
#include "ColorMixerCursor.hpp"
#include "PigmentButton.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include <memory>

namespace Beer::System
{
    void ColorMixerManager::Update()
    {
        if (!enabled)
        {
            return;
        }

        if (colorPicker != nullptr)
        {
            colorPicker->Update();
        }

        if (colorMixerCursor != nullptr)
        {
            MouseInput mouseInput = getMouseInput();

            colorMixerCursor->Update(mouseInput.PixelPos, mouseInput.leftClickStart);

            if (colorMixerCursor->IsActive())
            {
                markDirty();
            }
        }
    }

    void ColorMixerManager::AddPigmentButton(UITransform* transform,
        Rendering::Material* spriteMaterial,
        PigmentType pigment)
    {
        Function<void, PigmentType, UITransform*, bool> pigmentCallback = [this](PigmentType pigment, UITransform* transform, bool clicked) -> void {
            SetCurrentPigment(pigment, transform);

            if (clicked)
            {
                selectClip->Play();
            }
        };

        pigmentButtons.emplace_back(std::make_unique<PigmentButton>(transform,
            spriteMaterial,
            pigment,
            pigmentCallback));
    }

    void ColorMixerManager::SetCurrentPigment(PigmentType pigment, UITransform* transform)
    {
        currentPigment = pigment;
        colorPicker->PaintPigment();
        transform->BindChild(selectSpriteTransform);
        markDirty();
        OnPigmentClicked.Invoke(pigment);
    }

    void ColorMixerManager::SetCurrentPigmentByIndex(uint32_t index)
    {
        index = std::clamp<uint32_t>(index, 0, pigmentButtons.size());
        pigmentButtons[index]->ClickedCallback(false);
    }

    void ColorMixerManager::SetClearButton(Function<void> clearColorMixer,
        UITransform* clearTransform,
        Rendering::Material* clearMaterial)
    {
        this->clearColorMixer = clearColorMixer;

        clearButton = std::make_unique<Button>(clearTransform, clearMaterial);

        clearButton->SetOnClick([this]() -> void {
            ClearColorMixer();
            clearCanvasClip->Play();
        });
    }

    void ColorMixerManager::SetColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
        Function<MouseInput> getMouseInput,
        UITransform* canvasTransform)
    {
        colorPicker = std::make_unique<ColorPicker>(subscribeToReadback,
            getMouseInput,
            canvasTransform);
    }

    void ColorMixerManager::SetColorMixerCursor(UITransform* canvasTransform,
        UITransform* cursorTransform,
        Rendering::Material* cursorMaterial,
        Rendering::Texture2D* brushTexture,
        Rendering::Texture2D* brushMask,
        Rendering::Texture2D* pickerTexture,
        Rendering::Texture2D* pickerMask)
    {
        colorMixerCursor = std::make_unique<ColorMixerCursor>(canvasTransform,
            cursorTransform,
            cursorMaterial,
            brushTexture,
            brushMask,
            pickerTexture,
            pickerMask);

        colorMixerCursor->SetCursor(MixerCursorType::Picker, GetPigmentColor(currentPigment));

        Function<void, PigmentType> setBrushCursor = [this](PigmentType pigment) -> void {
            colorMixerCursor->SetCursor(MixerCursorType::Brush, GetPigmentColor(pigment));
        };

        Function<void, glm::vec4> setPickerCursor = [this](glm::vec4 color) -> void {
            colorMixerCursor->SetCursor(MixerCursorType::Picker, color);
        };

        OnPigmentClicked.Subscribe(setBrushCursor);
        colorPicker->OnColorPicked.Subscribe(setPickerCursor);
    }

    void ColorMixerManager::ClearColorMixer()
    {
        if (clearColorMixer == nullptr)
            return;

        clearColorMixer();
    }
} // namespace Beer::System
