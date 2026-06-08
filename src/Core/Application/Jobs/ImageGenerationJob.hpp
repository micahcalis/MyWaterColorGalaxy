#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/Threads.hpp"

namespace Beer::Core
{
    class ImageGenerationJob : public IUploadJob
    {
    private:
        std::shared_ptr<Rendering::Image> image;
        Rendering::ComputeContext* computeContext;
        Rendering::Threads threads;
        uint32_t kernelIndex;

    public:
        ImageGenerationJob(std::shared_ptr<Rendering::Image> image,
            Rendering::ComputeContext* computeContext,
            Rendering::Threads threads,
            uint32_t kernelIndex)
            : image(image), computeContext(computeContext), threads(threads), kernelIndex(kernelIndex), IUploadJob(nullptr, 0)
        {
        }

        void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset) override;
    };
} // namespace Beer::Core
