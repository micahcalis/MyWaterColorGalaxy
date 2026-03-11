#pragma once

#include "System/Components/Registry/GameEntity.hpp"
#include "System/Galaxy/Player/PlayerInput.hpp"
#include "System/Delegates/Delegate.hpp"

namespace Beer::System
{
    struct PlayerEntity : public GameEntity
    {
    private:
        Function<PlayerInput> getPlayerInput;

    public:
        PlayerEntity(Transform transform,
            RenderComponent renderComponent,
            Function<PlayerInput> getPlayerInput)
            : getPlayerInput(getPlayerInput), GameEntity(transform, renderComponent)
        {
        }

        void Update() override;

    private:
        void InitializeManager() override;
    };
} // namespace Beer::System
