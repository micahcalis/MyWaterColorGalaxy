#pragma once

#include "System/Audio/AudioAsset.hpp"
#include <memory>
#include <string>

namespace Beer::Core
{
    class AudioManager;
}

namespace Beer::System
{
    struct AudioSettings
    {
    public:
        float Volume = 1.0f;
    };

    class AudioClip
    {
    private:
        inline static Core::AudioManager* audioManager = nullptr;

    private:
        std::shared_ptr<System::AudioAsset> asset = nullptr;
        AudioSettings settings{};

    public:
        static void SetAudioManager(Core::AudioManager* audioManager)
        {
            AudioClip::audioManager = audioManager;
        }

        AudioClip(std::shared_ptr<System::AudioAsset> asset,
            AudioSettings settings);

        AudioClip(const std::string& assetName,
            AudioSettings settings);

        AudioSettings& GetSettings()
        {
            return settings;
        }

        void Play();
        void PlayAsBackground();
    };
} // namespace Beer::System
