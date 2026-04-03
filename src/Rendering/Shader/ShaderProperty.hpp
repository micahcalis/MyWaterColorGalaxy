#pragma once

#include "vulkan/vulkan.hpp"

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
        StructuredBuffer,
        RWStructuredBuffer
    };

    struct ShaderProperty
    {
    public:
        PropertyType Type;
        uint32_t Offset;
        uint32_t Size;
        uint32_t Binding;
    };
} // namespace Beer::Rendering
