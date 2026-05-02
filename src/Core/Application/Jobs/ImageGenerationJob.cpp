#include "Core/Application/Jobs/ImageGenerationJob.hpp"
#include "Rendering/Texture/Texture2D.hpp"

namespace Beer::Core
{
    static const std::string GENERATION_TARGET = "_GenerationTarget";

    void ImageGenerationJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        Rendering::Texture2D tempTexture = Rendering::Texture2D(image);
        computeContext->SetTexture(GENERATION_TARGET, &tempTexture);
        computeContext->Update();

        image->QueueImageGenerate(commandBuffer,
            computeContext,
            threads,
            kernelIndex);
    }
} // namespace Beer::Core
