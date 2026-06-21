#include "Core/Application/Managers/AudioManager.hpp"
#include "AudioManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "System/Audio/AudioAsset.hpp"
#include <format>
#include <memory>
#include <stdexcept>
#include "System/Audio/LoopingStream.hpp"

namespace Beer::Core
{
    AudioManager::~AudioManager()
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

        if (audioDevice != 0)
        {
            SDL_CloseAudioDevice(audioDevice);
        }

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

    AudioManager::AudioManager()
    {
        SDL_InitSubSystem(SDL_INIT_AUDIO);

        audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (audioDevice == 0)
        {
            throw std::runtime_error(SDL_GetError());
        }
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
        UpdateClips();
        UpdateLoops();
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

    std::shared_ptr<System::LoopingStream> AudioManager::GetLoopingStream(std::shared_ptr<System::AudioAsset> audioAsset, const System::AudioSettings& settings)
    {
        std::shared_ptr<System::LoopingStream> loopingStream(new System::LoopingStream(audioDevice,
            audioAsset,
            settings));

        activeLoops.push_back(std::weak_ptr<System::LoopingStream>(loopingStream));

        return loopingStream;
    }

    const std::filesystem::path AudioManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetAudioAssetPath(name);
    }

    void AudioManager::UpdateClips()
    {
        std::erase_if(clipStreams, [](const auto& stream) -> bool {
            bool clipFinished = SDL_GetAudioStreamAvailable(stream) <= 0;

            if (clipFinished)
            {
                SDL_DestroyAudioStream(stream);
            }

            return clipFinished;
        });
    }

    void AudioManager::UpdateLoops()
    {
        std::erase_if(activeLoops, [](const std::weak_ptr<System::LoopingStream>& weakLoop) -> bool {
            if (auto loop = weakLoop.lock())
            {
                loop->Update();
                return false;
            }

            return true;
        });
    }
} // namespace Beer::Core
