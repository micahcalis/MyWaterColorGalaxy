#include "System/PaintTool/ColorMixer/ColorPicker.hpp"
#include "ColorPicker.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include <print>

namespace Beer::System
{
    void ColorPicker::Update()
    {
        switch (state)
        {
        case ColorPickingState::Idle: return;
        case ColorPickingState::Waiting: return;
        case ColorPickingState::Picking: TryPickColor(); break;
        }
    }

    void ColorPicker::ClickButton()
    {
        switch (state)
        {
        case ColorPickingState::Idle:
            state = ColorPickingState::Picking;
            break;
        case ColorPickingState::Waiting: return;
        case ColorPickingState::Picking:
            state = ColorPickingState::Idle;
            break;
        }
    }

    void ColorPicker::TryPickColor()
    {
        MouseInput mouseInput = getMouseInput();

        if (!mouseInput.leftClickStart)
            return;

        PixelRect rect = canvasTransform->Rect;
        glm::vec2 pos = mouseInput.PixelPos;

        if (pos.x < rect.BotLeft.x
            || pos.y < rect.BotLeft.y
            || pos.x >= rect.TopRight.x
            || pos.y >= rect.TopRight.y)
        {
            return;
        }

        glm::vec2 normalizedCanvasPos = GetNormalizedCanvasPos(rect, pos);

        currentPickingPos = glm::vec2(normalizedCanvasPos.x * Rendering::SIMULATION_RES_X,
            normalizedCanvasPos.y * Rendering::SIMULATION_RES_Y);

        subscribeToReadback([this](ImagePixelData imageData) -> void { SelectColorReadback(imageData); });
        state = ColorPickingState::Waiting;
    }

    void ColorPicker::SelectColorReadback(ImagePixelData imageData)
    {
        int baseX = static_cast<int>(currentPickingPos.x);
        int baseY = static_cast<int>(currentPickingPos.y);
        glm::vec4 colorSum = glm::vec4(0);

        for (int y = baseY - 1; y <= baseY + 1; y++)
        {
            for (int x = baseX - 1; x <= baseX + 1; x++)
            {
                Pixel pixel = imageData.GetPixel(x, y);
                colorSum += glm::vec4(pixel.Red, pixel.Green, pixel.Blue, pixel.Alpha);
            }
        }

        colorSum /= 9.0f;

        pickedColor = glm::vec4(colorSum);
        state = ColorPickingState::Idle;
        OnColorPicked.Invoke(pickedColor);
    }

    glm::vec2 ColorPicker::GetNormalizedCanvasPos(const PixelRect& rect, const glm::vec2 mousePos) const
    {
        float x = (mousePos.x - rect.BotLeft.x) / (rect.BotRight.x - rect.BotLeft.x);
        float y = (mousePos.y - rect.BotLeft.y) / (rect.TopLeft.y - rect.BotLeft.y);
        return glm::vec2(x, y);
    }
} // namespace Beer::System
