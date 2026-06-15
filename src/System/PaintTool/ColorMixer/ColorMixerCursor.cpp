#include "System/PaintTool/ColorMixer/ColorMixerCursor.hpp"
#include "ColorMixerCursor.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/PaintTool/ColorMixer/ColorPicker.hpp"

namespace Beer::System
{
    ColorMixerCursor::ColorMixerCursor(UITransform* canvasTransform,
        UITransform* cursorTransform,
        Rendering::Material* cursorMaterial,
        Rendering::Texture2D* brushTexture,
        Rendering::Texture2D* brushMask,
        Rendering::Texture2D* pickerTexture,
        Rendering::Texture2D* pickerMask)
        : canvasTransform(canvasTransform)
        , cursorTransform(cursorTransform)
        , cursorMaterial(cursorMaterial)
        , brushTexture(brushTexture)
        , brushMask(brushMask)
        , pickerTexture(pickerTexture)
        , pickerMask(pickerMask)
    {
    }

    void ColorMixerCursor::Update(glm::vec2 mousePixelPos)
    {
        isActive = QuadCollider::Hit(canvasTransform, mousePixelPos);
        cursorTransform->Position = (mousePixelPos - canvasTransform->Rect.BotLeft) / Core::Screen::RectUnitLength();
        cursorMaterial->SetInt("_ActiveState", isActive ? 1 : 0);
    }

    void ColorMixerCursor::SetCursor(MixerCursorType state, glm::vec4 color)
    {
        Rendering::Texture2D* sprite = state == MixerCursorType::Picker ? pickerTexture : brushTexture;
        Rendering::Texture2D* mask = state == MixerCursorType::Picker ? pickerMask : brushMask;

        cursorMaterial->SetTexture("_SpriteTex", sprite, false);
        cursorMaterial->SetTexture("_MaskTex", mask, false);
        cursorMaterial->SetColor("_ActiveColor", color);
    }

} // namespace Beer::System
