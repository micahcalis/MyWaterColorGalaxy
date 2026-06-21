#pragma once

#include "SDL3/SDL_audio.h"
#include <memory>
#include "System/Audio/AudioAsset.hpp"
#include "System/Audio/AudioClip.hpp"

namespace Beer::System
{
    class LoopingStream
    {
        friend class Core::AudioManager;

    private:
        SDL_AudioDeviceID audioDevice = 0;
        std::shared_ptr<System::AudioAsset> asset = nullptr;
        SDL_AudioStream* stream = nullptr;

    public:
        ~LoopingStream();
        void Update();
        void Stop();

    private:
        LoopingStream(SDL_AudioDeviceID device, std::shared_ptr<System::AudioAsset> audioAsset, const System::AudioSettings& settings);
    };
} // namespace Beer::System
