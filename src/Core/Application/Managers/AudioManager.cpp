#include "Core/Application/Managers/AudioManager.hpp"
#include "AudioManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "System/Audio/AudioAsset.hpp"
#include <format>
#include <stdexcept>

namespace Beer::Core
{
    void BackgroundPlayer::SetMusic(std::shared_ptr<System::AudioAsset> asset,
        const System::AudioSettings& settings)
    {
        if (bgAsset != nullptr)
        {
            SDL_DestroyAudioStream(bgStream);
            bgStream = nullptr;
        }

        bgAsset = asset;

        if (bgAsset == nullptr)
            return;

        bgStream = SDL_CreateAudioStream(asset->Spec(), nullptr);
        SDL_BindAudioStream(audioDevice, bgStream);

        SDL_SetAudioStreamGain(bgStream, settings.Volume);
        SDL_PutAudioStreamData(bgStream, asset->Data(), asset->Length());
        SDL_FlushAudioStream(bgStream);
    }

    void BackgroundPlayer::Update()
    {
        if (bgAsset == nullptr
            || bgStream == nullptr)
        {
            return;
        }

        if (SDL_GetAudioStreamAvailable(bgStream) < bgAsset->Length())
        {
            SDL_PutAudioStreamData(bgStream, bgAsset->Data(), bgAsset->Length());
        }
    }

    void BackgroundPlayer::Stop()
    {
        if (bgStream != nullptr)
        {
            SDL_DestroyAudioStream(bgStream);
        }

        bgStream = nullptr;
        bgAsset = nullptr;
    }

    AudioManager::AudioManager()
    {
        SDL_InitSubSystem(SDL_INIT_AUDIO);

        audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (audioDevice == 0)
        {
            throw std::runtime_error(SDL_GetError());
        }

        bgPlayer = std::make_unique<BackgroundPlayer>(audioDevice);
    }

    std::shared_ptr<System::AudioAsset> AudioManager::Load(const std::filesystem::path& path)
    {
        SDL_AudioSpec wavSpec;
        Uint8* wavData = nullptr;
        Uint32 wavLength = 0;

        if (!SDL_LoadWAV(path.string().c_str(), &wavSpec, &wavData, &wavLength))
        {
            throw std::runtime_error(std::format("Error loading WAV at: {}", path.string()));
        }

        return std::make_shared<System::AudioAsset>(
            wavSpec,
            wavData,
            wavLength);
    }

    void AudioManager::Update()
    {
        std::erase_if(clipStreams, [](const auto& stream) -> bool {
            bool clipFinished = SDL_GetAudioStreamAvailable(stream) <= 0;

            if (clipFinished)
            {
                SDL_DestroyAudioStream(stream);
            }

            return clipFinished;
        });

        bgPlayer->Update();
    }

    void AudioManager::PlayOnce(System::AudioAsset* asset, const System::AudioSettings& settings)
    {
        SDL_AudioStream* stream = SDL_CreateAudioStream(asset->Spec(), nullptr);
        SDL_BindAudioStream(audioDevice, stream);
        SDL_SetAudioStreamGain(stream, settings.Volume);
        SDL_PutAudioStreamData(stream, asset->Data(), asset->Length());

        SDL_FlushAudioStream(stream);
        clipStreams.push_back(stream);
    }

    void AudioManager::SetBackgroundAsset(std::shared_ptr<System::AudioAsset> asset,
        const System::AudioSettings& settings)
    {
        bgPlayer->SetMusic(std::move(asset), settings);
    }

    const std::filesystem::path AudioManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetAudioAssetPath(name);
    }

} // namespace Beer::Core
