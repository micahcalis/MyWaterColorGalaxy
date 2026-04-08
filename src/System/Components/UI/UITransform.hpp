#pragma once

#include "glm/glm.hpp"
#include "Rendering/Shader/RectPush.hpp"

namespace Beer::System
{
    struct PixelRect
    {
        constexpr static float DEG2RAD = 57.2957795131f;

    public:
        glm::vec2 TopRight = glm::vec2(0.5f, 0.5f);
        glm::vec2 BotRight = glm::vec2(0.5f, -0.5f);
        glm::vec2 TopLeft = glm::vec2(-0.5f, 0.5f);
        glm::vec2 BotLeft = glm::vec2(-0.5f, -0.5f);

        PixelRect() = default;

        PixelRect(glm::vec2 tr, glm::vec2 br, glm::vec2 tl, glm::vec2 bl)
            : TopRight(tr), BotRight(br), TopLeft(tl), BotLeft(bl)
        {
        }

    public:
        void Scale(const glm::vec2 scale)
        {
            TopRight *= scale;
            BotRight *= scale;
            TopLeft *= scale;
            BotLeft *= scale;
        }

        void Rotate(const float rotation)
        {
            TopRight = Rotate2D(TopRight, rotation);
            BotRight = Rotate2D(BotRight, rotation);
            TopLeft = Rotate2D(TopLeft, rotation);
            BotLeft = Rotate2D(BotLeft, rotation);
        }

        void Move(const glm::vec2 offset)
        {
            TopRight += offset;
            BotRight += offset;
            TopLeft += offset;
            BotLeft += offset;
        }

        [[nodiscard]] Rendering::RectPush GetRectPush()
        {
            return Rendering::RectPush();
        }

    private:
        static glm::vec2 Rotate2D(glm::vec2 in, float degrees)
        {
            float radians = degrees * DEG2RAD;
            float s = glm::sin(radians);
            float c = glm::cos(radians);
            glm::mat2x2 rotMat = glm::mat2x2(c, -s, s, c);
            return rotMat * in;
        }
    };

    enum class AnchorMode : int32_t
    {
        Center = 0,
        BottomLeft = 1,
        TopLeft = 2,
        BottomRight = 3,
        TopRight = 4,
        BottomMiddle = 5,
        TopMiddle = 6,
        MiddleLeft = 7,
        MiddleRight = 8
    };

    struct UITransform
    {
    public:
        UITransform* Parent = nullptr;
        AnchorMode Anchor;
        AnchorMode Pivot;
        glm::vec2 Position = glm::vec2(0);
        float Depth = 0;
        float Rotation = 0;
        glm::vec2 Scale = glm::vec2(1);
        PixelRect Rect{};

    public:
        glm::vec2 GetPixelAnchor(const AnchorMode mode) const;
        void CalculatePixelRect();

    private:
        static glm::vec2 GetPivotOffset(AnchorMode pivot);
    };
} // namespace Beer::System
