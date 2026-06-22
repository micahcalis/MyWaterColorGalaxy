#pragma once

#include "PlayerInput.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Audio/SoundGlobalSettings.hpp"

namespace Beer::System
{
    class PlayerEntity;

    class PlayerController
    {
    private:
        PlayerEntity* player;
        float speed = 0;
        std::shared_ptr<AudioClip> boostClip = nullptr;
        std::shared_ptr<LoopingStream> loopingStream = nullptr;
        bool wasBoosting = false;

    public:
        PlayerController(PlayerEntity* player)
            : player(player)
        {
            AudioSettings audioSettings{};
            audioSettings.Volume = BOOST_VOLUME;
            boostClip = std::make_shared<AudioClip>("SoundEffects/Game/Audio_Boost",
                audioSettings);
        }

        void Update(PlayerInput playerInput);
        void Move(PlayerInput playerInput);
        float GetNormalizedSpeed() const;
        static void SanitizeMouseInput(glm::vec2& mouseVec);

    private:
        void HandleSpeed(bool isBoosting);
        void HandleBoostAudio(bool isBoosting);
    };
} // namespace Beer::System
