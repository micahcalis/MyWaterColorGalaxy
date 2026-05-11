#include "System/PaintTool/HologramCursor/HologramCursorManager.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include <print>

namespace Beer::System
{
    static const glm::vec4 ACTIVE_COLOR = glm::vec4(0.3f, 1.0f, 0.35f, 0.5f);
    static const glm::vec4 INACTIVE_COLOR = glm::vec4(1.0f, 0.35f, 0.35f, 0.5f);

    void HologramCursorManager::SetTransform(glm::vec2 mousePos, float brushSize)
    {
        glm::vec2 screenPos = mousePos / Core::Screen::RectUnitLength();
        cursorTransform->Position = screenPos;
        cursorTransform->Scale = glm::vec2(brushSize);
    }

    void HologramCursorManager::SetMaterial(bool canUseCursor, GalaxyBrushType brush)
    {
        cursorMaterial->SetColor("_TintColor", canUseCursor ? ACTIVE_COLOR : INACTIVE_COLOR);
        cursorMaterial->SetTexture("_SpriteTex", getBrushTexture(brush), false);
    }

} // namespace Beer::System
