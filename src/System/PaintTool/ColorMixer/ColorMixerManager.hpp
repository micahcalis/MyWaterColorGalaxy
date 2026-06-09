#pragma once

#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "System/PaintTool/ColorMixer/PigmentButton.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include <vector>

namespace Beer::System
{
    class ColorMixerManager : public IEntityManager
    {
    private:
        std::vector<std::unique_ptr<PigmentButton>> pigmentButtons;
        PigmentType currentPigment = PigmentType::QuinacridoneRose;
        std::unique_ptr<Button> clearButton = nullptr;
        Function<void> clearColorMixer = nullptr;
        std::unique_ptr<ColorPicker> colorPicker = nullptr;

    public:
        void Update() override;

        void AddPigmentButton(UITransform* transform,
            Rendering::Material* spriteMaterial,
            PigmentType pigment);

        void SetCurrentPigment(PigmentType pigment);
        PigmentType GetCurrentPigment() const { return currentPigment; }

        void SetClearButton(Function<void> clearColorMixer,
            UITransform* clearTransform,
            Rendering::Material* clearMaterial);

        void SetColorPicker(Function<void, Function<void, ImagePixelData>> subscribeToReadback,
            Function<MouseInput> getMouseInput,
            UITransform* colorPickerTransform,
            Rendering::Material* colorPickerMaterial,
            UITransform* paintPigmentTransform,
            Rendering::Material* paintPigmentMaterial,
            UITransform* canvasTransform);

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

    private:
        void ClearColorMixer();
        void CloseColorMixer();
    };
} // namespace Beer::System
