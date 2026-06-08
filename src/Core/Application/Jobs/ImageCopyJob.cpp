#include "Core/Application/Jobs/ImageCopyJob.hpp"

namespace Beer::Core
{
    void ImageCopyJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        sourceImage->QueueImageCopy(commandBuffer,
            destinationImage,
            sourceLayout);
    }
} // namespace Beer::Core
