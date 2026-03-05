#pragma once

#include "System/ECS/ECS.hpp"
#include "System/ECS//Components/Transform.hpp"

namespace Beer::System
{
    struct Player
    {
        EntityHandle Handle;
        Transform* Transform = nullptr;
    };
} // namespace Beer::System
