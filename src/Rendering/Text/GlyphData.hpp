#pragma once

namespace Beer::Rendering
{
    struct GlyphRect
    {
    public:
        float Left;
        float Bottom;
        float Right;
        float Top;
    };

    struct GlyphData
    {
    public:
        float Advance;
        GlyphRect CharacterBounds;
        GlyphRect AtlasBounds;
    };
} // namespace Beer::Rendering
