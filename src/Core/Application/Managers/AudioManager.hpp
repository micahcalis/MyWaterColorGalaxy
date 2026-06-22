#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_init.h"
#include "System/Audio/AudioAsset.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Audio/LoopingStream.hpp"

namespace Beer::Core
{
    class AudioManager : public IAssetManager<System::AudioAsset>
    {
    private:
        SDL_AudioDeviceID audioDevice = 0;
        std::vector<SDL_AudioStream*> clipStreams;
        std::vector<std::weak_ptr<System::LoopingStream>> activeLoops;

    public:
        ~AudioManager();
        AudioManager();
        std::shared_ptr<System::AudioAsset> Load(const std::filesystem::path& path) override;
        void Update();
        void PlayOnce(System::AudioAsset* asset, const System::AudioSettings& settings);
        std::shared_ptr<System::LoopingStream> GetLoopingStream(std::shared_ptr<System::AudioAsset> audioAsset, const System::AudioSettings& settings);

    private:
        const std::filesystem::path GetPath(const std::string& name) override;
        void UpdateClips();
        void UpdateLoops();
    };
} // namespace Beer::Core
