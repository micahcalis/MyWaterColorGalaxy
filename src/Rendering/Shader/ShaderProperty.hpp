#pragma once

#include "Rendering/Texture/Texture2D.hpp"
#include <cstdint>

namespace Beer::Rendering
{
    enum class PropertyType
    {
        Unknown = 0,
        Int,
        Float,
        Vector2,
        Vector3,
        Vector4,
        Matrix4x4,
        Texture2D,
        RWTexture2D,
        Texture2DArray,
        RWTexture2DArray,
        Texture3D,
        RWTexture3D,
        StructuredBuffer,
        RWStructuredBuffer
    };

    struct ShaderProperty
    {
    public:
        PropertyType Type = PropertyType::Unknown;
        uint32_t Offset = 0;
        uint32_t Size = 0;
        uint32_t Binding = 0;
        bool Dynamic = false;
    };
} // namespace Beer::Rendering
