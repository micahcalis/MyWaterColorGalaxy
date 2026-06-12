#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "SDL3/SDL_audio.h"
#include "System/Audio/AudioAsset.hpp"
#include "System/Audio/AudioClip.hpp"

namespace Beer::Core
{
    class BackgroundPlayer
    {
    private:
        SDL_AudioDeviceID audioDevice = 0;
        std::shared_ptr<System::AudioAsset> bgAsset = nullptr;
        SDL_AudioStream* bgStream = nullptr;

    public:
        ~BackgroundPlayer()
        {
            Stop();
        }

        BackgroundPlayer(SDL_AudioDeviceID audioDevice)
            : audioDevice(audioDevice)
        {
        }

        void SetMusic(std::shared_ptr<System::AudioAsset> asset,
            const System::AudioSettings& settings);

        void Update();
        void Stop();
    };

    class AudioManager : public IAssetManager<System::AudioAsset>
    {
    private:
        SDL_AudioDeviceID audioDevice = 0;
        std::vector<SDL_AudioStream*> clipStreams;
        std::unique_ptr<BackgroundPlayer> bgPlayer = nullptr;

    public:
        ~AudioManager()
        {
            for (SDL_AudioStream* stream : clipStreams)
            {
                if (stream != nullptr)
                {
                    SDL_DestroyAudioStream(stream);
                }
            }
            clipStreams.clear();

            Clear();
        }

        AudioManager();
        std::shared_ptr<System::AudioAsset> Load(const std::filesystem::path& path) override;
        void Update();
        void PlayOnce(System::AudioAsset* asset, const System::AudioSettings& settings);

        void SetBackgroundAsset(std::shared_ptr<System::AudioAsset> asset,
            const System::AudioSettings& settings);

    private:
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
