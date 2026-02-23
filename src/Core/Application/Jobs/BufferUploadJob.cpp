#include "Core/Application/Jobs/BufferUploadJob.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include <memory>

namespace Beer::Core
{
    void BufferUploadJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        stagingBuffer->Upload(data, Size, offset);
        stagingBuffer->QueueCopyTo(*buffer, commandBuffer, Size, offset);
    }
} // namespace Beer::Core
