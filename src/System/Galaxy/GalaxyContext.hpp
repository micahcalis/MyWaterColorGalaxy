#pragma once

#include "System/Context/IContext.hpp"
#include "System/Default/SingleStaticEntity.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include <vector>

namespace Beer::System
{
    class GalaxyContext : public IContext
    {
    private:
        PlayerEntity* playerEntity;
        std::vector<SingleStaticEntity*> staticEntities;
        Function<PlayerInput> getPlayerInput;

    public:
        GalaxyContext(Function<PlayerInput> getPlayerInput);
        void Load() override;
        void Update() override;
    };
} // namespace Beer::System
