#pragma once

#include "System/Context/IContext.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    private:
        PlayerEntity* playerEntity;
        Function<PlayerInput> getPlayerInput;

    public:
        GalaxyContext(Function<PlayerInput> getPlayerInput);
        void Load() override;
        void Update() override;
    };
} // namespace Beer::System
