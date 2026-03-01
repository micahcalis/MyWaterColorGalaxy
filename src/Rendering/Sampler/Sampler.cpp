#include "Rendering/Sampler/Sampler.hpp"
#include "Rendering/Sampler/SamplerCache.hpp"

namespace Beer::Rendering
{
    std::shared_ptr<Sampler> Sampler::Get(vk::Filter filter,
        vk::SamplerAddressMode tiling,
        float maxAnisotropy)
    {
        return samplerCache->GetSampler(SamplerKey(filter, tiling, maxAnisotropy));
    }
} // namespace Beer::Rendering
