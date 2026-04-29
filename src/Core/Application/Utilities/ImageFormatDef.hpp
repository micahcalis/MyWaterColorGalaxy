#pragma once

#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    enum class Channel
    {
        Red,
        Green,
        Blue,
        Alpha,
        None
    };

    enum class ChannelType
    {
        Unorm8,
        Snorm8,
        Uint8,
        Sint8,
        Unorm16,
        Snorm16,
        Uint16,
        Sint16,
        Float16,
        Uint32,
        Sint32,
        Float32,
        Uint64,
        Sint64,
        Float64,
        Packed,
        Unknown
    };

    constexpr std::array<Channel, 4> GetChannelOrder(vk::Format format)
    {
        switch (format)
        {
        case vk::Format::eR4G4B4A4UnormPack16:
        case vk::Format::eR5G5B5A1UnormPack16:
        case vk::Format::eR8G8B8A8Unorm:
        case vk::Format::eR8G8B8A8Snorm:
        case vk::Format::eR8G8B8A8Uscaled:
        case vk::Format::eR8G8B8A8Sscaled:
        case vk::Format::eR8G8B8A8Uint:
        case vk::Format::eR8G8B8A8Sint:
        case vk::Format::eR8G8B8A8Srgb:
        case vk::Format::eR16G16B16A16Unorm:
        case vk::Format::eR16G16B16A16Snorm:
        case vk::Format::eR16G16B16A16Uscaled:
        case vk::Format::eR16G16B16A16Sscaled:
        case vk::Format::eR16G16B16A16Uint:
        case vk::Format::eR16G16B16A16Sint:
        case vk::Format::eR16G16B16A16Sfloat:
        case vk::Format::eR32G32B32A32Uint:
        case vk::Format::eR32G32B32A32Sint:
        case vk::Format::eR32G32B32A32Sfloat:
        case vk::Format::eR64G64B64A64Uint:
        case vk::Format::eR64G64B64A64Sint:
        case vk::Format::eR64G64B64A64Sfloat:
        case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:
        case vk::Format::eR10X6G10X6B10X6A10X6Uint4Pack16ARM:
        case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:
        case vk::Format::eR12X4G12X4B12X4A12X4Uint4Pack16ARM:
        case vk::Format::eR14X2G14X2B14X2A14X2Unorm4Pack16ARM:
        case vk::Format::eR14X2G14X2B14X2A14X2Uint4Pack16ARM:
            return {Channel::Red, Channel::Green, Channel::Blue, Channel::Alpha};

        case vk::Format::eB4G4R4A4UnormPack16:
        case vk::Format::eB5G5R5A1UnormPack16:
        case vk::Format::eB8G8R8A8Unorm:
        case vk::Format::eB8G8R8A8Snorm:
        case vk::Format::eB8G8R8A8Uscaled:
        case vk::Format::eB8G8R8A8Sscaled:
        case vk::Format::eB8G8R8A8Uint:
        case vk::Format::eB8G8R8A8Sint:
        case vk::Format::eB8G8R8A8Srgb:
            return {Channel::Blue, Channel::Green, Channel::Red, Channel::Alpha};

        case vk::Format::eA1R5G5B5UnormPack16:
        case vk::Format::eA4R4G4B4UnormPack16:
        case vk::Format::eA2R10G10B10UnormPack32:
        case vk::Format::eA2R10G10B10SnormPack32:
        case vk::Format::eA2R10G10B10UscaledPack32:
        case vk::Format::eA2R10G10B10SscaledPack32:
        case vk::Format::eA2R10G10B10UintPack32:
        case vk::Format::eA2R10G10B10SintPack32:
            return {Channel::Alpha, Channel::Red, Channel::Green, Channel::Blue};

        case vk::Format::eA1B5G5R5UnormPack16:
        case vk::Format::eA4B4G4R4UnormPack16:
        case vk::Format::eA8B8G8R8UnormPack32:
        case vk::Format::eA8B8G8R8SnormPack32:
        case vk::Format::eA8B8G8R8UscaledPack32:
        case vk::Format::eA8B8G8R8SscaledPack32:
        case vk::Format::eA8B8G8R8UintPack32:
        case vk::Format::eA8B8G8R8SintPack32:
        case vk::Format::eA8B8G8R8SrgbPack32:
        case vk::Format::eA2B10G10R10UnormPack32:
        case vk::Format::eA2B10G10R10SnormPack32:
        case vk::Format::eA2B10G10R10UscaledPack32:
        case vk::Format::eA2B10G10R10SscaledPack32:
        case vk::Format::eA2B10G10R10UintPack32:
        case vk::Format::eA2B10G10R10SintPack32:
            return {Channel::Alpha, Channel::Blue, Channel::Green, Channel::Red};

        case vk::Format::eR5G6B5UnormPack16:
        case vk::Format::eR8G8B8Unorm:
        case vk::Format::eR8G8B8Snorm:
        case vk::Format::eR8G8B8Uscaled:
        case vk::Format::eR8G8B8Sscaled:
        case vk::Format::eR8G8B8Uint:
        case vk::Format::eR8G8B8Sint:
        case vk::Format::eR8G8B8Srgb:
        case vk::Format::eR16G16B16Unorm:
        case vk::Format::eR16G16B16Snorm:
        case vk::Format::eR16G16B16Uscaled:
        case vk::Format::eR16G16B16Sscaled:
        case vk::Format::eR16G16B16Uint:
        case vk::Format::eR16G16B16Sint:
        case vk::Format::eR16G16B16Sfloat:
        case vk::Format::eR32G32B32Uint:
        case vk::Format::eR32G32B32Sint:
        case vk::Format::eR32G32B32Sfloat:
        case vk::Format::eR64G64B64Sint:
        case vk::Format::eR64G64B64Sfloat:
            return {Channel::Red, Channel::Green, Channel::Blue, Channel::None};

        case vk::Format::eB5G6R5UnormPack16:
        case vk::Format::eB8G8R8Unorm:
        case vk::Format::eB8G8R8Snorm:
        case vk::Format::eB8G8R8Uscaled:
        case vk::Format::eB8G8R8Sscaled:
        case vk::Format::eB8G8R8Uint:
        case vk::Format::eB8G8R8Sint:
        case vk::Format::eB8G8R8Srgb:
        case vk::Format::eB10G11R11UfloatPack32:
        case vk::Format::eE5B9G9R9UfloatPack32:
            return {Channel::Blue, Channel::Green, Channel::Red, Channel::None};

        case vk::Format::eR4G4UnormPack8:
        case vk::Format::eR8G8Unorm:
        case vk::Format::eR8G8Snorm:
        case vk::Format::eR8G8Uscaled:
        case vk::Format::eR8G8Sscaled:
        case vk::Format::eR8G8Uint:
        case vk::Format::eR8G8Sint:
        case vk::Format::eR8G8Srgb:
        case vk::Format::eR16G16Unorm:
        case vk::Format::eR16G16Snorm:
        case vk::Format::eR16G16Uscaled:
        case vk::Format::eR16G16Sscaled:
        case vk::Format::eR16G16Uint:
        case vk::Format::eR16G16Sint:
        case vk::Format::eR16G16Sfloat:
        case vk::Format::eR32G32Uint:
        case vk::Format::eR32G32Sint:
        case vk::Format::eR32G32Sfloat:
        case vk::Format::eR64G64Uint:
        case vk::Format::eR64G64Sint:
        case vk::Format::eR64G64Sfloat:
        case vk::Format::eR16G16Sfixed5NV:
        case vk::Format::eR10X6G10X6Unorm2Pack16:
        case vk::Format::eR10X6G10X6Uint2Pack16ARM:
        case vk::Format::eR12X4G12X4Unorm2Pack16:
        case vk::Format::eR12X4G12X4Uint2Pack16ARM:
        case vk::Format::eR14X2G14X2Unorm2Pack16ARM:
        case vk::Format::eR14X2G14X2Uint2Pack16ARM:
            return {Channel::Red, Channel::Green, Channel::None, Channel::None};

        case vk::Format::eR8Unorm:
        case vk::Format::eR8Snorm:
        case vk::Format::eR8Uscaled:
        case vk::Format::eR8Sscaled:
        case vk::Format::eR8Uint:
        case vk::Format::eR8Sint:
        case vk::Format::eR8Srgb:
        case vk::Format::eS8Uint:
        case vk::Format::eR8BoolARM:
        case vk::Format::eR16Unorm:
        case vk::Format::eR16Snorm:
        case vk::Format::eR16Uscaled:
        case vk::Format::eR16Sscaled:
        case vk::Format::eR16Uint:
        case vk::Format::eR16Sint:
        case vk::Format::eR16Sfloat:
        case vk::Format::eD16Unorm:
        case vk::Format::eR10X6UnormPack16:
        case vk::Format::eR10X6UintPack16ARM:
        case vk::Format::eR12X4UnormPack16:
        case vk::Format::eR12X4UintPack16ARM:
        case vk::Format::eR14X2UnormPack16ARM:
        case vk::Format::eR14X2UintPack16ARM:
        case vk::Format::eR32Uint:
        case vk::Format::eR32Sint:
        case vk::Format::eR32Sfloat:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eR64Uint:
        case vk::Format::eR64Sint:
        case vk::Format::eR64Sfloat:
        case vk::Format::eD32SfloatS8Uint:
            return {Channel::Red, Channel::None, Channel::None, Channel::None};

        default:
            return {Channel::None, Channel::None, Channel::None, Channel::None};
        }
    }

    constexpr size_t GetBytesPerPixel(vk::Format format)
    {
        switch (format)
        {
        case vk::Format::eR4G4UnormPack8:
        case vk::Format::eR8Unorm:
        case vk::Format::eR8Snorm:
        case vk::Format::eR8Uscaled:
        case vk::Format::eR8Sscaled:
        case vk::Format::eR8Uint:
        case vk::Format::eR8Sint:
        case vk::Format::eR8Srgb:
        case vk::Format::eS8Uint:
        case vk::Format::eR8BoolARM:
            return 1;

        case vk::Format::eR4G4B4A4UnormPack16:
        case vk::Format::eB4G4R4A4UnormPack16:
        case vk::Format::eR5G6B5UnormPack16:
        case vk::Format::eB5G6R5UnormPack16:
        case vk::Format::eR5G5B5A1UnormPack16:
        case vk::Format::eB5G5R5A1UnormPack16:
        case vk::Format::eA1R5G5B5UnormPack16:
        case vk::Format::eA1B5G5R5UnormPack16:
        case vk::Format::eA4R4G4B4UnormPack16:
        case vk::Format::eA4B4G4R4UnormPack16:
        case vk::Format::eR8G8Unorm:
        case vk::Format::eR8G8Snorm:
        case vk::Format::eR8G8Uscaled:
        case vk::Format::eR8G8Sscaled:
        case vk::Format::eR8G8Uint:
        case vk::Format::eR8G8Sint:
        case vk::Format::eR8G8Srgb:
        case vk::Format::eR16Unorm:
        case vk::Format::eR16Snorm:
        case vk::Format::eR16Uscaled:
        case vk::Format::eR16Sscaled:
        case vk::Format::eR16Uint:
        case vk::Format::eR16Sint:
        case vk::Format::eR16Sfloat:
        case vk::Format::eD16Unorm:
        case vk::Format::eR10X6UnormPack16:
        case vk::Format::eR10X6UintPack16ARM:
        case vk::Format::eR12X4UnormPack16:
        case vk::Format::eR12X4UintPack16ARM:
        case vk::Format::eR14X2UnormPack16ARM:
        case vk::Format::eR14X2UintPack16ARM:
            return 2;

        case vk::Format::eR8G8B8Unorm:
        case vk::Format::eR8G8B8Snorm:
        case vk::Format::eR8G8B8Uscaled:
        case vk::Format::eR8G8B8Sscaled:
        case vk::Format::eR8G8B8Uint:
        case vk::Format::eR8G8B8Sint:
        case vk::Format::eR8G8B8Srgb:
        case vk::Format::eB8G8R8Unorm:
        case vk::Format::eB8G8R8Snorm:
        case vk::Format::eB8G8R8Uscaled:
        case vk::Format::eB8G8R8Sscaled:
        case vk::Format::eB8G8R8Uint:
        case vk::Format::eB8G8R8Sint:
        case vk::Format::eB8G8R8Srgb:
            return 3;

        case vk::Format::eR8G8B8A8Unorm:
        case vk::Format::eR8G8B8A8Snorm:
        case vk::Format::eR8G8B8A8Uscaled:
        case vk::Format::eR8G8B8A8Sscaled:
        case vk::Format::eR8G8B8A8Uint:
        case vk::Format::eR8G8B8A8Sint:
        case vk::Format::eR8G8B8A8Srgb:
        case vk::Format::eB8G8R8A8Unorm:
        case vk::Format::eB8G8R8A8Snorm:
        case vk::Format::eB8G8R8A8Uscaled:
        case vk::Format::eB8G8R8A8Sscaled:
        case vk::Format::eB8G8R8A8Uint:
        case vk::Format::eB8G8R8A8Sint:
        case vk::Format::eB8G8R8A8Srgb:
        case vk::Format::eA8B8G8R8UnormPack32:
        case vk::Format::eA8B8G8R8SnormPack32:
        case vk::Format::eA8B8G8R8UscaledPack32:
        case vk::Format::eA8B8G8R8SscaledPack32:
        case vk::Format::eA8B8G8R8UintPack32:
        case vk::Format::eA8B8G8R8SintPack32:
        case vk::Format::eA8B8G8R8SrgbPack32:
        case vk::Format::eA2R10G10B10UnormPack32:
        case vk::Format::eA2R10G10B10SnormPack32:
        case vk::Format::eA2R10G10B10UscaledPack32:
        case vk::Format::eA2R10G10B10SscaledPack32:
        case vk::Format::eA2R10G10B10UintPack32:
        case vk::Format::eA2R10G10B10SintPack32:
        case vk::Format::eA2B10G10R10UnormPack32:
        case vk::Format::eA2B10G10R10SnormPack32:
        case vk::Format::eA2B10G10R10UscaledPack32:
        case vk::Format::eA2B10G10R10SscaledPack32:
        case vk::Format::eA2B10G10R10UintPack32:
        case vk::Format::eA2B10G10R10SintPack32:
        case vk::Format::eR16G16Unorm:
        case vk::Format::eR16G16Snorm:
        case vk::Format::eR16G16Uscaled:
        case vk::Format::eR16G16Sscaled:
        case vk::Format::eR16G16Uint:
        case vk::Format::eR16G16Sint:
        case vk::Format::eR16G16Sfloat:
        case vk::Format::eR32Uint:
        case vk::Format::eR32Sint:
        case vk::Format::eR32Sfloat:
        case vk::Format::eB10G11R11UfloatPack32:
        case vk::Format::eE5B9G9R9UfloatPack32:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eR16G16Sfixed5NV:
        case vk::Format::eR10X6G10X6Unorm2Pack16:
        case vk::Format::eR10X6G10X6Uint2Pack16ARM:
        case vk::Format::eR12X4G12X4Unorm2Pack16:
        case vk::Format::eR12X4G12X4Uint2Pack16ARM:
        case vk::Format::eR14X2G14X2Unorm2Pack16ARM:
        case vk::Format::eR14X2G14X2Uint2Pack16ARM:
            return 4;

        case vk::Format::eR16G16B16Unorm:
        case vk::Format::eR16G16B16Snorm:
        case vk::Format::eR16G16B16Uscaled:
        case vk::Format::eR16G16B16Sscaled:
        case vk::Format::eR16G16B16Uint:
        case vk::Format::eR16G16B16Sint:
        case vk::Format::eR16G16B16Sfloat:
            return 6;

        case vk::Format::eR16G16B16A16Unorm:
        case vk::Format::eR16G16B16A16Snorm:
        case vk::Format::eR16G16B16A16Uscaled:
        case vk::Format::eR16G16B16A16Sscaled:
        case vk::Format::eR16G16B16A16Uint:
        case vk::Format::eR16G16B16A16Sint:
        case vk::Format::eR16G16B16A16Sfloat:
        case vk::Format::eR32G32Uint:
        case vk::Format::eR32G32Sint:
        case vk::Format::eR32G32Sfloat:
        case vk::Format::eR64Uint:
        case vk::Format::eR64Sint:
        case vk::Format::eR64Sfloat:
        case vk::Format::eD32SfloatS8Uint:
        case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:
        case vk::Format::eR10X6G10X6B10X6A10X6Uint4Pack16ARM:
        case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:
        case vk::Format::eR12X4G12X4B12X4A12X4Uint4Pack16ARM:
        case vk::Format::eR14X2G14X2B14X2A14X2Unorm4Pack16ARM:
        case vk::Format::eR14X2G14X2B14X2A14X2Uint4Pack16ARM:
            return 8;

        case vk::Format::eR32G32B32Uint:
        case vk::Format::eR32G32B32Sint:
        case vk::Format::eR32G32B32Sfloat:
            return 12;

        case vk::Format::eR32G32B32A32Uint:
        case vk::Format::eR32G32B32A32Sint:
        case vk::Format::eR32G32B32A32Sfloat:
        case vk::Format::eR64G64Uint:
        case vk::Format::eR64G64Sint:
        case vk::Format::eR64G64Sfloat:
            return 16;

        case vk::Format::eR64G64B64Sint:
        case vk::Format::eR64G64B64Sfloat:
            return 24;

        case vk::Format::eR64G64B64A64Uint:
        case vk::Format::eR64G64B64A64Sint:
        case vk::Format::eR64G64B64A64Sfloat:
            return 32;

        default:
            return 0;
        }
    }

    constexpr ChannelType GetChannelType(vk::Format format)
    {
        switch (format)
        {
        case vk::Format::eR8Unorm:
        case vk::Format::eR8G8Unorm:
        case vk::Format::eR8G8B8Unorm:
        case vk::Format::eB8G8R8Unorm:
        case vk::Format::eR8G8B8A8Unorm:
        case vk::Format::eB8G8R8A8Unorm:
        case vk::Format::eR8Srgb:
        case vk::Format::eR8G8Srgb:
        case vk::Format::eR8G8B8Srgb:
        case vk::Format::eB8G8R8Srgb:
        case vk::Format::eR8G8B8A8Srgb:
        case vk::Format::eB8G8R8A8Srgb:
            return ChannelType::Unorm8;

        case vk::Format::eR8Snorm:
        case vk::Format::eR8G8Snorm:
        case vk::Format::eR8G8B8Snorm:
        case vk::Format::eB8G8R8Snorm:
        case vk::Format::eR8G8B8A8Snorm:
        case vk::Format::eB8G8R8A8Snorm:
            return ChannelType::Snorm8;

        case vk::Format::eR8Uint:
        case vk::Format::eR8G8Uint:
        case vk::Format::eR8G8B8Uint:
        case vk::Format::eB8G8R8Uint:
        case vk::Format::eR8G8B8A8Uint:
        case vk::Format::eB8G8R8A8Uint:
        case vk::Format::eS8Uint:
        case vk::Format::eR8Uscaled:
        case vk::Format::eR8G8Uscaled:
        case vk::Format::eR8G8B8Uscaled:
        case vk::Format::eB8G8R8Uscaled:
        case vk::Format::eR8G8B8A8Uscaled:
        case vk::Format::eB8G8R8A8Uscaled:
        case vk::Format::eR8BoolARM:
            return ChannelType::Uint8;

        case vk::Format::eR8Sint:
        case vk::Format::eR8G8Sint:
        case vk::Format::eR8G8B8Sint:
        case vk::Format::eB8G8R8Sint:
        case vk::Format::eR8G8B8A8Sint:
        case vk::Format::eB8G8R8A8Sint:
        case vk::Format::eR8Sscaled:
        case vk::Format::eR8G8Sscaled:
        case vk::Format::eR8G8B8Sscaled:
        case vk::Format::eB8G8R8Sscaled:
        case vk::Format::eR8G8B8A8Sscaled:
        case vk::Format::eB8G8R8A8Sscaled:
            return ChannelType::Sint8;

        case vk::Format::eR16Unorm:
        case vk::Format::eR16G16Unorm:
        case vk::Format::eR16G16B16Unorm:
        case vk::Format::eR16G16B16A16Unorm:
        case vk::Format::eD16Unorm:
            return ChannelType::Unorm16;

        case vk::Format::eR16Snorm:
        case vk::Format::eR16G16Snorm:
        case vk::Format::eR16G16B16Snorm:
        case vk::Format::eR16G16B16A16Snorm:
            return ChannelType::Snorm16;

        case vk::Format::eR16Uint:
        case vk::Format::eR16G16Uint:
        case vk::Format::eR16G16B16Uint:
        case vk::Format::eR16G16B16A16Uint:
        case vk::Format::eR16Uscaled:
        case vk::Format::eR16G16Uscaled:
        case vk::Format::eR16G16B16Uscaled:
        case vk::Format::eR16G16B16A16Uscaled:
            return ChannelType::Uint16;

        case vk::Format::eR16Sint:
        case vk::Format::eR16G16Sint:
        case vk::Format::eR16G16B16Sint:
        case vk::Format::eR16G16B16A16Sint:
        case vk::Format::eR16Sscaled:
        case vk::Format::eR16G16Sscaled:
        case vk::Format::eR16G16B16Sscaled:
        case vk::Format::eR16G16B16A16Sscaled:
            return ChannelType::Sint16;

        case vk::Format::eR16Sfloat:
        case vk::Format::eR16G16Sfloat:
        case vk::Format::eR16G16B16Sfloat:
        case vk::Format::eR16G16B16A16Sfloat:
            return ChannelType::Float16;

        case vk::Format::eR32Uint:
        case vk::Format::eR32G32Uint:
        case vk::Format::eR32G32B32Uint:
        case vk::Format::eR32G32B32A32Uint:
            return ChannelType::Uint32;

        case vk::Format::eR32Sint:
        case vk::Format::eR32G32Sint:
        case vk::Format::eR32G32B32Sint:
        case vk::Format::eR32G32B32A32Sint:
            return ChannelType::Sint32;

        case vk::Format::eR32Sfloat:
        case vk::Format::eR32G32Sfloat:
        case vk::Format::eR32G32B32Sfloat:
        case vk::Format::eR32G32B32A32Sfloat:
        case vk::Format::eD32Sfloat:
            return ChannelType::Float32;

        case vk::Format::eR64Uint:
        case vk::Format::eR64G64Uint:
        case vk::Format::eR64G64B64Uint:
        case vk::Format::eR64G64B64A64Uint:
            return ChannelType::Uint64;

        case vk::Format::eR64Sint:
        case vk::Format::eR64G64Sint:
        case vk::Format::eR64G64B64Sint:
        case vk::Format::eR64G64B64A64Sint:
            return ChannelType::Sint64;

        case vk::Format::eR64Sfloat:
        case vk::Format::eR64G64Sfloat:
        case vk::Format::eR64G64B64Sfloat:
        case vk::Format::eR64G64B64A64Sfloat:
            return ChannelType::Float64;

        case vk::Format::eR4G4UnormPack8:
        case vk::Format::eR4G4B4A4UnormPack16:
        case vk::Format::eB4G4R4A4UnormPack16:
        case vk::Format::eR5G6B5UnormPack16:
        case vk::Format::eB5G6R5UnormPack16:
        case vk::Format::eR5G5B5A1UnormPack16:
        case vk::Format::eB5G5R5A1UnormPack16:
        case vk::Format::eA1R5G5B5UnormPack16:
        case vk::Format::eA1B5G5R5UnormPack16:
        case vk::Format::eA4R4G4B4UnormPack16:
        case vk::Format::eA4B4G4R4UnormPack16:
        case vk::Format::eA8B8G8R8UnormPack32:
        case vk::Format::eA8B8G8R8SnormPack32:
        case vk::Format::eA8B8G8R8UscaledPack32:
        case vk::Format::eA8B8G8R8SscaledPack32:
        case vk::Format::eA8B8G8R8UintPack32:
        case vk::Format::eA8B8G8R8SintPack32:
        case vk::Format::eA8B8G8R8SrgbPack32:
        case vk::Format::eA2R10G10B10UnormPack32:
        case vk::Format::eA2R10G10B10SnormPack32:
        case vk::Format::eA2R10G10B10UscaledPack32:
        case vk::Format::eA2R10G10B10SscaledPack32:
        case vk::Format::eA2R10G10B10UintPack32:
        case vk::Format::eA2R10G10B10SintPack32:
        case vk::Format::eA2B10G10R10UnormPack32:
        case vk::Format::eA2B10G10R10SnormPack32:
        case vk::Format::eA2B10G10R10UscaledPack32:
        case vk::Format::eA2B10G10R10SscaledPack32:
        case vk::Format::eA2B10G10R10UintPack32:
        case vk::Format::eA2B10G10R10SintPack32:
        case vk::Format::eB10G11R11UfloatPack32:
        case vk::Format::eE5B9G9R9UfloatPack32:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD32SfloatS8Uint:
        case vk::Format::eR16G16Sfixed5NV:
        case vk::Format::eR10X6UnormPack16:
        case vk::Format::eR10X6UintPack16ARM:
        case vk::Format::eR12X4UnormPack16:
        case vk::Format::eR12X4UintPack16ARM:
        case vk::Format::eR14X2UnormPack16ARM:
        case vk::Format::eR14X2UintPack16ARM:
        case vk::Format::eR10X6G10X6Unorm2Pack16:
        case vk::Format::eR10X6G10X6Uint2Pack16ARM:
        case vk::Format::eR12X4G12X4Unorm2Pack16:
        case vk::Format::eR12X4G12X4Uint2Pack16ARM:
        case vk::Format::eR14X2G14X2Unorm2Pack16ARM:
        case vk::Format::eR14X2G14X2Uint2Pack16ARM:
        case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16:
        case vk::Format::eR10X6G10X6B10X6A10X6Uint4Pack16ARM:
        case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16:
        case vk::Format::eR12X4G12X4B12X4A12X4Uint4Pack16ARM:
        case vk::Format::eR14X2G14X2B14X2A14X2Unorm4Pack16ARM:
        case vk::Format::eR14X2G14X2B14X2A14X2Uint4Pack16ARM:
            return ChannelType::Packed;

        default:
            return ChannelType::Unknown;
        }
    }
} // namespace Beer::Core
