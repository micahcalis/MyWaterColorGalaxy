#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include <memory>

namespace Beer::System
{
    class TransitionMaterialGetter
    {
    public:
        static Rendering::TransitionInitialization GetHyperspaceInitialization();
    };
} // namespace Beer::System
