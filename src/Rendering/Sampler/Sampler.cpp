#include "Rendering/Sampler/Sampler.hpp"
#include "Core/Application/Renderer/RenderGarbageCollector.hpp"
#include "Rendering/Sampler/SamplerCache.hpp"

namespace Beer::Rendering
{
    Sampler::~Sampler()
    {
        Core::RenderGarbageCollector::Push(
            [deadSampler = std::move(sampler)]() -> void {});
    }

    std::shared_ptr<Sampler> Sampler::Get(vk::Filter filter,
        vk::SamplerAddressMode tiling,
        float maxAnisotropy)
    {
        return samplerCache->GetSampler(SamplerKey(filter, tiling, maxAnisotropy));
    }
} // namespace Beer::Rendering
