#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"

namespace Beer::System
{
    enum class MixerCursorType
    {
        Picker,
        Brush
    };

    class ColorMixerCursor
    {
    private:
        UITransform* canvasTransform = nullptr;
        UITransform* cursorTransform = nullptr;
        Rendering::Material* cursorMaterial = nullptr;
        Rendering::Texture2D* brushTexture = nullptr;
        Rendering::Texture2D* brushMask = nullptr;
        Rendering::Texture2D* pickerTexture = nullptr;
        Rendering::Texture2D* pickerMask = nullptr;
        bool isActive = false;

    public:
        ColorMixerCursor(UITransform* canvasTransform,
            UITransform* cursorTransform,
            Rendering::Material* cursorMaterial,
            Rendering::Texture2D* brushTexture,
            Rendering::Texture2D* brushMask,
            Rendering::Texture2D* pickerTexture,
            Rendering::Texture2D* pickerMask);

        void Update(glm::vec2 mousePixelPos);
        void SetCursor(MixerCursorType type, glm::vec4 color);
        bool IsActive() const { return isActive; }
    };
} // namespace Beer::System
