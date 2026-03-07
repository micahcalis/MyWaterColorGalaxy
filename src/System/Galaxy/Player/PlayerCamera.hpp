#pragma once

#include "System/Camera/Camera.hpp"
#include "System/Galaxy/Player/PlayerHandle.hpp"
#include <memory>
namespace Beer::System
{
    class PlayerCamera
    {
    private:
        const PlayerHandle player;
        std::unique_ptr<Camera> camera = nullptr;

    public:
        PlayerCamera(PlayerHandle player);
        void Update();

    private:
        void FollowPlayer();
    };
} // namespace Beer::System
