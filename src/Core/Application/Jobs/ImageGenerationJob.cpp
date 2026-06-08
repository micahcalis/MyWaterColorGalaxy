#include "Core/Application/Jobs/ImageGenerationJob.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <stdexcept>

namespace Beer::Core
{
    static const std::string GENERATION_TARGET = "_GenerationTarget";

    void ImageGenerationJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        if (image->GetData().Type == Rendering::TextureType::TwoDim)
        {
            if (!image->GetData().Cubemap)
            {
                Rendering::Texture2D tempTexture = Rendering::Texture2D(image);
                computeContext->SetTexture(GENERATION_TARGET, &tempTexture);
                computeContext->SetVector("_Resolution", glm::vec4(tempTexture.GetWidth(), tempTexture.GetHeight(), 0, 0));
            } else
            {
            }
        } else if (image->GetData().Type == Rendering::TextureType::ThreeDim)
        {
            Rendering::Texture3D tempTexture = Rendering::Texture3D(image);
            computeContext->SetTexture(GENERATION_TARGET, &tempTexture);
            computeContext->SetVector("_Resolution", glm::vec4(tempTexture.GetWidth(), tempTexture.GetHeight(), tempTexture.GetDepth(), 0));
        } else
        {
            throw std::runtime_error(std::format("Unsupported Texture Type for Image Generation Job: {}",
                magic_enum::enum_name(image->GetData().Type)));
        }

        computeContext->Update();

        image->QueueImageGenerate(commandBuffer,
            computeContext,
            threads,
            kernelIndex);
    }
} // namespace Beer::Core
