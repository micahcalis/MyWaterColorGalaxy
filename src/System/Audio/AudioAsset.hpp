#pragma once

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_stdinc.h"
#include "Core/Application/Utilities/macros.hpp"

namespace Beer::System
{
    class AudioAsset
    {
    private:
        SDL_AudioSpec wavSpec;
        Uint8* wavData = nullptr;
        Uint32 wavLength = 0;

    public:
        ~AudioAsset()
        {
            SDL_free(wavData);
        }

        AudioAsset(SDL_AudioSpec wavSpec,
            Uint8* wavData,
            Uint32 wavLength)
            : wavSpec(wavSpec)
            , wavData(wavData)
            , wavLength(wavLength)
        {
        }

        NO_COPY(AudioAsset)

        AudioAsset(AudioAsset&& other) noexcept
            : wavSpec(other.wavSpec), wavData(other.wavData), wavLength(other.wavLength)
        {
            other.wavData = nullptr;
            other.wavLength = 0;
        }

        AudioAsset& operator=(AudioAsset&& other) noexcept
        {
            if (this != &other)
            {
                if (wavData)
                    SDL_free(wavData);
                wavSpec = other.wavSpec;
                wavData = other.wavData;
                wavLength = other.wavLength;

                other.wavData = nullptr;
                other.wavLength = 0;
            }
            return *this;
        }

        const SDL_AudioSpec* Spec() const { return &wavSpec; }
        Uint8* Data() const { return wavData; }
        Uint32 Length() const { return wavLength; }
    };
} // namespace Beer::System
