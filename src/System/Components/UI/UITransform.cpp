#include "System/Components/UI/UITransform.hpp"
#include "Core/Application/Renderer/Screen.hpp"

#include "UITransform.hpp"

namespace Beer::System
{
    glm::vec2 UITransform::GetPixelAnchor(const AnchorMode mode) const
    {
        switch (mode)
        {
        case AnchorMode::Center: return (Rect.BotLeft + Rect.BotRight + Rect.TopLeft + Rect.TopRight) * 0.25f;
        case AnchorMode::BottomLeft: return Rect.BotLeft;
        case AnchorMode::TopLeft: return Rect.TopLeft;
        case AnchorMode::BottomRight: return Rect.BotRight;
        case AnchorMode::TopRight: return Rect.TopRight;
        case AnchorMode::BottomMiddle: return (Rect.BotRight + Rect.BotLeft) * 0.5f;
        case AnchorMode::TopMiddle: return (Rect.TopRight + Rect.TopLeft) * 0.5f;
        case AnchorMode::MiddleLeft: return (Rect.TopLeft + Rect.BotLeft) * 0.5f;
        case AnchorMode::MiddleRight: return (Rect.TopRight + Rect.BotRight) * 0.5f;
        }
    }

    void UITransform::CalculatePixelRect()
    {
        glm::vec2 anchorOffset = Parent->GetPixelAnchor(Anchor);

        PixelRect unitRect{};
        unitRect.Scale(Scale);
        unitRect.Rotate(Rotation);
        unitRect.Scale(glm::vec2(Core::Screen::RectUnitLength()));

        glm::vec2 unitOffset = Position + GetPivotOffset(Pivot) * Scale;
        unitOffset *= Core::Screen::RectUnitLength();
        unitRect.Move(unitOffset + anchorOffset);

        Rect = unitRect;
    }

    glm::vec2 UITransform::GetPivotOffset(AnchorMode pivot)
    {
        switch (pivot)
        {
        case AnchorMode::Center: return glm::vec2(0);
        case AnchorMode::BottomLeft: return glm::vec2(-0.5f, -0.5f);
        case AnchorMode::TopLeft: return glm::vec2(-0.5f, 0.5f);
        case AnchorMode::BottomRight: return glm::vec2(0.5f, -0.5f);
        case AnchorMode::TopRight: return glm::vec2(0.5f, 0.5f);
        case AnchorMode::BottomMiddle: return glm::vec2(0, -0.5f);
        case AnchorMode::TopMiddle: return glm::vec2(0, 0.5f);
        case AnchorMode::MiddleLeft: return glm::vec2(-0.5f, 0);
        case AnchorMode::MiddleRight: return glm::vec2(0.5f, 0);
        }
    }
} // namespace Beer::System
