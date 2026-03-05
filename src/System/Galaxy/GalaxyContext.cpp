#pragma once

#include "System/Galaxy/GalaxyContext.hpp"
#include "System/Context/IContext.hpp"
#include "System/ECS/ECS.hpp"
#include "System/ECS/Registry.hpp"

namespace Beer::System
{
    GalaxyContext::GalaxyContext()
    {
        Load();
    }

    void GalaxyContext::Update()
    {
        IContext::Update();

        // do galaxy stuff
    }

    void GalaxyContext::Load()
    {
        Entity player = registry.Create();
    }
} // namespace Beer::System
