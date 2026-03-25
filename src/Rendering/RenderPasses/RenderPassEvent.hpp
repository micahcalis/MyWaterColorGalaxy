#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class RenderPassEvent : uint32_t
    {
        SKYBOX = 100,
        OPAQUE = 200,
        END = 1000
    };
}
