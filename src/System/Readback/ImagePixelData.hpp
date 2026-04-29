#pragma once

#include "Core/Application/Utilities/ImageFormatDef.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "Rendering/Buffer/ImageData.hpp"
#include "vulkan/vulkan.hpp"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include "glm/glm.hpp"

namespace Beer::System
{
    struct Pixel
    {
        float Red = 0;
        float Green = 0;
        float Blue = 0;
        float Alpha = 0;

    public:
        void SetValueByType(Core::Channel channel, float value)
        {
            switch (channel)
            {
            case Core::Channel::Red: Red = value; break;
            case Core::Channel::Green: Green = value; break;
            case Core::Channel::Blue: Blue = value; break;
            case Core::Channel::Alpha: Alpha = value; break;
            case Core::Channel::None: return;
            }
        }
    };

    struct ImagePixelData
    {
    private:
        Core::ImageFormatData formatData;
        uint32_t numChannels = 0;
        vk::Extent3D extent;
        void* mappedData;

    public:
        ImagePixelData(Rendering::ImageData data, void* mappedData)
            : formatData(Core::ImageUtilities::GetFormatData(vk::Format(data.Format))), extent(data.Extent), mappedData(mappedData)
        {
            for (const auto& channel : formatData.Channels)
            {
                if (channel != Core::Channel::None)
                    numChannels++;
            }
        }

        Pixel GetPixel(uint32_t x, uint32_t y, uint32_t z = 0)
        {
            if (numChannels == 0)
            {
                throw std::runtime_error("Trying to read pixels from image with no channels");
            }

            if (OutOfBounds(x, y, z))
            {
                x = std::min(x, extent.width - 1);
                y = std::min(y, extent.height - 1);
                z = std::min(z, extent.depth - 1);
            }

            const uint8_t* pixels = static_cast<uint8_t*>(mappedData);
            Pixel pixel{};
            size_t baseOffset = (x + (y * extent.width) + (z * extent.width * extent.height)) * formatData.PixelSize;
            size_t channelOffset = formatData.PixelSize / numChannels;

            CastChannels(pixels, baseOffset, pixel);

            return pixel;
        }

    private:
        bool OutOfBounds(uint32_t x, uint32_t y, uint32_t z)
        {
            return x >= extent.width
                || y >= extent.height
                || z >= extent.depth;
        }

        void CastChannels(const uint8_t* pixels, size_t baseOffset, Pixel& pixel)
        {
            switch (formatData.ChannelType)
            {
            case Core::ChannelType::Packed:
            case Core::ChannelType::Unknown:
                throw std::runtime_error("Trying to read from Packed or Unknown Image Format");

            case Core::ChannelType::Unorm8: {
                const uint8_t* channels = reinterpret_cast<const uint8_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    float value = static_cast<float>(channels[i]) / 255.0f;
                    pixel.SetValueByType(formatData.Channels[i], value);
                }
                break;
            }
            case Core::ChannelType::Snorm8: {
                const int8_t* channels = reinterpret_cast<const int8_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    float value = std::max(static_cast<float>(channels[i]) / 127.0f, -1.0f);
                    pixel.SetValueByType(formatData.Channels[i], value);
                }
                break;
            }
            case Core::ChannelType::Uint8: {
                const uint8_t* channels = reinterpret_cast<const uint8_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            case Core::ChannelType::Sint8: {
                const int8_t* channels = reinterpret_cast<const int8_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }

            case Core::ChannelType::Unorm16: {
                const uint16_t* channels = reinterpret_cast<const uint16_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    float value = static_cast<float>(channels[i]) / 65535.0f;
                    pixel.SetValueByType(formatData.Channels[i], value);
                }
                break;
            }
            case Core::ChannelType::Snorm16: {
                const int16_t* channels = reinterpret_cast<const int16_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    float value = std::max(static_cast<float>(channels[i]) / 32767.0f, -1.0f);
                    pixel.SetValueByType(formatData.Channels[i], value);
                }
                break;
            }
            case Core::ChannelType::Uint16: {
                const uint16_t* channels = reinterpret_cast<const uint16_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            case Core::ChannelType::Sint16: {
                const int16_t* channels = reinterpret_cast<const int16_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            case Core::ChannelType::Float16: {
                const uint16_t* channels = reinterpret_cast<const uint16_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    float value = glm::detail::toFloat32(channels[i]);
                    pixel.SetValueByType(formatData.Channels[i], value);
                }
                break;
            }

            case Core::ChannelType::Float32: {
                const float* channels = reinterpret_cast<const float*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], channels[i]);
                }
                break;
            }
            case Core::ChannelType::Uint32: {
                const uint32_t* channels = reinterpret_cast<const uint32_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    // Note: Massive uint32s will lose slight precision when cast to float
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            case Core::ChannelType::Sint32: {
                const int32_t* channels = reinterpret_cast<const int32_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }

            case Core::ChannelType::Float64: {
                const double* channels = reinterpret_cast<const double*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            case Core::ChannelType::Uint64: {
                const uint64_t* channels = reinterpret_cast<const uint64_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            case Core::ChannelType::Sint64: {
                const int64_t* channels = reinterpret_cast<const int64_t*>(pixels + baseOffset);
                for (int i = 0; i < numChannels; i++)
                {
                    pixel.SetValueByType(formatData.Channels[i], static_cast<float>(channels[i]));
                }
                break;
            }
            }
        }
    };
} // namespace Beer::System
