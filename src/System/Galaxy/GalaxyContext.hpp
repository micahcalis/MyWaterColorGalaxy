#pragma once

#include "System/Context/IContext.hpp"

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    public:
        GalaxyContext();
        void Load() override;
        void Update() override;
    };
} // namespace Beer::System
