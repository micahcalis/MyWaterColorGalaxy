#pragma once

namespace Beer::System
{
    enum class HorizontalAlignment
    {
        Left,
        Center,
        Right
    };

    enum class VerticalAlignment
    {
        Top,
        Middle,
        Bottom
    };

    enum class WrapMode
    {
        Overflow,
        Clip,
        Truncate
    };

    struct TextSettings
    {
    public:
        HorizontalAlignment HorizontalAlignment = HorizontalAlignment::Left;
        VerticalAlignment VerticalAlignment = VerticalAlignment::Top;
        WrapMode HorizontalWrapping = WrapMode::Truncate;
        WrapMode VerticalWrapping = WrapMode::Overflow;
        float CharacterSpacing = 0.0f;
        float LineSpacing = 1.0f;
    };
} // namespace Beer::System
