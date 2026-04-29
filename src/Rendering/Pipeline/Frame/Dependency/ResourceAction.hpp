#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class ResourceAction : uint32_t
    {
        None = 0,
        ColorRead = 1 << 0,
        ColorWrite = 1 << 1,
        DepthRead = 1 << 2,
        DepthWrite = 1 << 3,
        ComputeRead = 1 << 4,
        ComputeWrite = 1 << 5,
        ComputeReadWrite = 1 << 6,
        BufferRead = 1 << 7,
        BufferWrite = 1 << 8,
        BufferReadWrite = 1 << 9,
        ComputeBufferRead = 1 << 10,
        ComputeBufferWrite = 1 << 11,
        ComputeBufferReadWrite = 1 << 12,
        TransferRead = 1 << 13,
        All = ~0u
    };

    inline constexpr uint32_t operator|(ResourceAction a, ResourceAction b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, ResourceAction action)
    {
        return mask | static_cast<uint32_t>(action);
    }

    constexpr uint32_t RACTION_NONE_BITS = static_cast<uint32_t>(ResourceAction::None);
    constexpr uint32_t RACTION_CREAD_BITS = static_cast<uint32_t>(ResourceAction::ColorRead);
    constexpr uint32_t RACTION_CWRITE_BITS = static_cast<uint32_t>(ResourceAction::ColorWrite);
    constexpr uint32_t RACTION_ZREAD_BITS = static_cast<uint32_t>(ResourceAction::DepthRead);
    constexpr uint32_t RACTION_ZWRITE_BITS = static_cast<uint32_t>(ResourceAction::DepthWrite);
    constexpr uint32_t RACTION_COMPREAD_BITS = static_cast<uint32_t>(ResourceAction::ComputeRead);
    constexpr uint32_t RACTION_COMPWRITE_BITS = static_cast<uint32_t>(ResourceAction::ComputeWrite);
    constexpr uint32_t RACTION_COMPREADWRITE_BITS = static_cast<uint32_t>(ResourceAction::ComputeReadWrite);
    constexpr uint32_t RACTION_BUFREAD_BITS = static_cast<uint32_t>(ResourceAction::BufferRead);
    constexpr uint32_t RACTION_BUFWRITE_BITS = static_cast<uint32_t>(ResourceAction::BufferWrite);
    constexpr uint32_t RACTION_BUFREADWRITE_BITS = static_cast<uint32_t>(ResourceAction::BufferReadWrite);
    constexpr uint32_t RACTION_COMPBUFREAD_BITS = static_cast<uint32_t>(ResourceAction::ComputeBufferRead);
    constexpr uint32_t RACTION_COMPBUFWRITE_BITS = static_cast<uint32_t>(ResourceAction::ComputeWrite);
    constexpr uint32_t RACTION_COMPBUFREADWRITE_BITS = static_cast<uint32_t>(ResourceAction::ComputeBufferReadWrite);
    constexpr uint32_t RACTION_TRANSFERREAD_BITS = static_cast<uint32_t>(ResourceAction::TransferRead);
    constexpr uint32_t RACTION_ALL_BITS = static_cast<uint32_t>(ResourceAction::All);
} // namespace Beer::Rendering
