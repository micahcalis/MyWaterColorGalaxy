#include "System/Audio/LoopingStream.hpp"

namespace Beer::System
{
    LoopingStream::LoopingStream(SDL_AudioDeviceID device, std::shared_ptr<System::AudioAsset> audioAsset, const System::AudioSettings& settings)
        : audioDevice(device), asset(audioAsset)
    {
        if (asset == nullptr)
            return;

        stream = SDL_CreateAudioStream(asset->Spec(), nullptr);
        SDL_BindAudioStream(audioDevice, stream);
        SDL_SetAudioStreamGain(stream, settings.Volume);

        SDL_PutAudioStreamData(stream, asset->Data(), asset->Length());
        SDL_FlushAudioStream(stream);
    }

    LoopingStream::~LoopingStream()
    {
        Stop();
    }

    void LoopingStream::Update()
    {
        if (asset == nullptr || stream == nullptr)
            return;

        if (SDL_GetAudioStreamAvailable(stream) < asset->Length())
        {
            SDL_PutAudioStreamData(stream, asset->Data(), asset->Length());
        }
    }

    void LoopingStream::Stop()
    {
        if (stream != nullptr)
        {
            SDL_DestroyAudioStream(stream);
            stream = nullptr;
        }
    }
} // namespace Beer::System
