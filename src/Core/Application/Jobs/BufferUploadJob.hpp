#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include <memory>

namespace Beer::Core
{
    class BufferUploadJob : public IUploadJob
    {
    private:
        std::shared_ptr<Rendering::Buffer> buffer;

    public:
        BufferUploadJob(std::shared_ptr<Rendering::Buffer> buffer, void* data, VkDeviceSize size)
            : buffer(buffer), IUploadJob(data, size)
        {
        }

        void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset) override;
    };
} // namespace Beer::Core
