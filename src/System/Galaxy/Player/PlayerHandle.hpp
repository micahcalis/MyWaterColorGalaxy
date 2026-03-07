#pragma once

#include "System/ECS/ECS.hpp"
#include "System/ECS//Components/Transform.hpp"

namespace Beer::System
{
    struct PlayerHandle
    {
        EntityHandle Handle;
        Transform* Transform = nullptr;
    };
} // namespace Beer::System
