#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class RenderPassEvent : uint32_t
    {
        SKYBOX = 100,
        OPAQUE = 200,
        DEFERRED_SHADE = 300,
        PRE_USER_INTERFACE = 900,
        USER_INTERFACE = 950,
        END = 1000
    };
}
