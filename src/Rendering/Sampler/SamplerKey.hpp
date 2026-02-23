#pragma once

#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    struct SamplerKey
    {
    public:
        vk::Filter Filter;
        vk::SamplerAddressMode Tiling;
        float MaxAnisotropy;

        SamplerKey(vk::Filter mag, vk::Filter min, vk::SamplerAddressMode tiling, vk::Bool32 aniso, float maxAniso)
            : Filter(mag), Tiling(tiling), MaxAnisotropy(maxAniso)
        {
        }

        bool operator==(const SamplerKey& other) const
        {
            return Filter == other.Filter && Tiling == other.Tiling && MaxAnisotropy == other.MaxAnisotropy;
        }
    };
} // namespace Beer::Rendering

namespace std
{
    // AI Generated
    template<>
    struct hash<Beer::Rendering::SamplerKey>
    {
        std::size_t operator()(const Beer::Rendering::SamplerKey& k) const
        {
            // We use a common hashing technique called "Hash Combine"
            size_t res = 0;
            auto hash_combine = [&res](auto v) {
                res ^= std::hash<decltype(v)>{}(v) + 0x9e3779b9 + (res << 6) + (res >> 2);
            };

            hash_combine(static_cast<uint32_t>(k.Filter));
            hash_combine(static_cast<uint32_t>(k.Tiling));
            hash_combine(k.MaxAnisotropy);

            return res;
        }
    };
} // namespace std
