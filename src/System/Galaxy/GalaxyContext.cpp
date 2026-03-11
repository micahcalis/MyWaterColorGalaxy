#pragma once

#include "System/Galaxy/GalaxyContext.hpp"
#include "System/Components/General/RenderComponent.hpp"
#include "System/Context/IContext.hpp"
#include "System/Galaxy/Player/PlayerEntity.hpp"
#include "System/Components/Registry/Registry.hpp"
#include "System/Galaxy/Player/PlayerSettings.hpp"
#include "glm/ext/vector_float3.hpp"
#include <print>

namespace Beer::System
{
    GalaxyContext::GalaxyContext(Function<PlayerInput> getPlayerInput)
    {
        this->getPlayerInput = getPlayerInput;
    }

    void GalaxyContext::Update()
    {
        IContext::Update();
        playerEntity->Update();
    }

    void GalaxyContext::Load()
    {
        Transform transform{};
        transform.Position = PLAYER_SETTINGS.StartPos;
        playerEntity = registry.CreateEntity<PlayerEntity>(std::move(transform),
            RenderComponent(nullptr, nullptr),
            getPlayerInput);

        std::println("Created Player Entity");
    }
} // namespace Beer::System
