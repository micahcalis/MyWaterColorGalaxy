#include "System/PaintTool/ColorMixer/ColorMixerCursor.hpp"
#include "ColorMixerCursor.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Audio/SoundGlobalSettings.hpp"
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
        AudioSettings audioSettings{};
        audioSettings.Volume = PAINT_CLIP_VOLUME;
        paintClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_Paint",
            audioSettings);

        audioSettings.Volume = PICK_CLIP_VOLUME;
        pickClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_PickColor",
            audioSettings);
    }

    void ColorMixerCursor::Update(glm::vec2 mousePixelPos, bool isClicking)
    {
        isActive = QuadCollider::Hit(canvasTransform, mousePixelPos);
        cursorTransform->Position = (mousePixelPos - canvasTransform->Rect.BotLeft) / Core::Screen::RectUnitLength();
        cursorMaterial->SetInt("_ActiveState", isActive ? 1 : 0);

        if (isActive && isClicking)
        {
            switch (currentType)
            {
            case MixerCursorType::Brush: paintClip->Play(); break;
            case MixerCursorType::Picker: pickClip->Play(); break;
            }
        }
    }

    void ColorMixerCursor::SetCursor(MixerCursorType state, glm::vec4 color)
    {
        currentType = state;
        Rendering::Texture2D* sprite = state == MixerCursorType::Picker ? pickerTexture : brushTexture;
        Rendering::Texture2D* mask = state == MixerCursorType::Picker ? pickerMask : brushMask;

        cursorMaterial->SetTexture("_SpriteTex", sprite, false);
        cursorMaterial->SetTexture("_MaskTex", mask, false);
        cursorMaterial->SetColor("_ActiveColor", color);
    }

} // namespace Beer::System
