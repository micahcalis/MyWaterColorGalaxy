#include "Core/Application/Jobs/ImageClearJob.hpp"

namespace Beer::Core
{
    void ImageClearJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        image->QueueImageClear(commandBuffer, clearColor);
    }
} // namespace Beer::Core
