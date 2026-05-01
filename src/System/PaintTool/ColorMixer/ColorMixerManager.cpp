#include "System/PaintTool/ColorMixer/ColorMixerManager.hpp"
#include "PigmentButton.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <memory>

namespace Beer::System
{
    void ColorMixerManager::Update()
    {
        if (colorPicker != nullptr)
        {
            colorPicker->Update();
        }
    }

    void ColorMixerManager::AddPigmentButton(UITransform* transform,
        Rendering::Material* spriteMaterial,
        PigmentType pigment)
    {
        Function<void, PigmentType> setPigment = [this](PigmentType pigmentParam) -> void {
            SetCurrentPigment(pigmentParam);
        };

        pigmentButtons.emplace_back(std::make_unique<PigmentButton>(transform,
            spriteMaterial,
            pigment,
            setPigment));
    }

    void ColorMixerManager::SetCurrentPigment(PigmentType pigment)
    {
        currentPigment = pigment;
    }

    void ColorMixerManager::SetClearButton(Function<void> clearColorMixer,
        UITransform* clearTransform,
        Rendering::Material* clearMaterial)
    {
        this->clearColorMixer = clearColorMixer;

        clearButton = std::make_unique<Button>(clearTransform, clearMaterial);
        clearButton->SetOnClick([this]() -> void { ClearColorMixer(); });
    }

    void ColorMixerManager::SetCloseButton(Function<void> closeColorMixer,
        UITransform* closeTransform,
        Rendering::Material* closeMaterial)
    {
        this->closeColorMixer = closeColorMixer;

        closeButton = std::make_unique<Button>(closeTransform, closeMaterial);
        closeButton->SetOnClick([this]() -> void { CloseColorMixer(); });
    }

    void ColorMixerManager::SetColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
        Function<MouseInput> getMouseInput,
        UITransform* buttonTransform,
        Rendering::Material* buttonMaterial,
        UITransform* canvasTransform)
    {
        colorPicker = std::make_unique<ColorPicker>(subscribeToReadback,
            getMouseInput,
            buttonTransform,
            buttonMaterial,
            canvasTransform);
    }

    void ColorMixerManager::ClearColorMixer()
    {
        if (clearColorMixer == nullptr)
            return;

        clearColorMixer();
    }

    void ColorMixerManager::CloseColorMixer()
    {
        if (closeColorMixer == nullptr)
            return;

        closeColorMixer();
    }
} // namespace Beer::System
