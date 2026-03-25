#pragma once

#include <cstdint>

namespace Beer::Rendering
{
    enum class ReallocationFlag : uint32_t
    {
        None = 0,
        Image = 1 << 0,
        Sampler = 1 << 1,
        All = ~0u
    };

    inline constexpr uint32_t operator|(ReallocationFlag a, ReallocationFlag b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, ReallocationFlag bindType)
    {
        return mask | static_cast<uint32_t>(bindType);
    }

    constexpr uint32_t REALLOC_NONE_BITS = static_cast<uint32_t>(ReallocationFlag::None);
    constexpr uint32_t REALLOC_IMAGE_BITS = static_cast<uint32_t>(ReallocationFlag::Image);
    constexpr uint32_t REALLOC_SAMPLER_BITS = static_cast<uint32_t>(ReallocationFlag::Sampler);
    constexpr uint32_t REALLOC_ALL_BITS = static_cast<uint32_t>(ReallocationFlag::All);
} // namespace Beer::Rendering

namespace Beer::Rendering
{
    struct ReallocationMask
    {
    private:
        uint32_t bits;

    public:
        ReallocationMask(uint32_t bits)
            : bits(bits)
        {
        }

        ReallocationMask(ReallocationFlag layer)
            : bits(static_cast<uint32_t>(layer))
        {
        }

        [[nodiscard]] bool Has(const ReallocationFlag layer) const
        {
            return (bits & static_cast<uint32_t>(layer)) != 0;
        }
    };
} // namespace Beer::Rendering
