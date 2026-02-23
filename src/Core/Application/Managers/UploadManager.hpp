#pragma once

#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Core/Application/Jobs/IUploadJob.hpp"
#include <memory>

namespace Beer::Core
{
    class UploadManager
    {
    private:
        const Device& device;
        std::shared_ptr<Rendering::BufferAllocator> bufferAllocator;
        std::vector<std::unique_ptr<IUploadJob>> jobQueue;

    public:
        UploadManager(std::shared_ptr<Rendering::BufferAllocator> bufferAllocator,
            const Device& device)
            : bufferAllocator(bufferAllocator), device(device)
        {
        }
        void AddJob(std::unique_ptr<IUploadJob> job);
        void FlushQueue(const FrameResource& frameResource);

    private:
        size_t GetTotalQueueSize();
    };
} // namespace Beer::Core
