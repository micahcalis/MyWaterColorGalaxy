#include "System/Audio/AudioClip.hpp"
#include <print>
#include <stdexcept>
#include <Core/Application/Managers/AudioManager.hpp>

namespace Beer::System
{
    AudioClip::AudioClip(std::shared_ptr<System::AudioAsset> asset,
        AudioSettings settings)
        : asset(asset)
        , settings(std::move(settings))
    {
    }

    AudioClip::AudioClip(const std::string& assetName,
        AudioSettings settings)
        : settings(std::move(settings))
    {
        if (audioManager == nullptr)
        {
            throw std::runtime_error("trying to load Audio Asset when AudioManager is uninitialized");
        }

        asset = audioManager->Get(assetName);
    }

    void AudioClip::Play()
    {
        audioManager->PlayOnce(asset.get(), settings);
    }

    void AudioClip::PlayAsBackground()
    {
        audioManager->SetBackgroundAsset(asset, settings);
    }
} // namespace Beer::System
