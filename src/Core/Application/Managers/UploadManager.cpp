#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include <memory>

namespace Beer::Core
{
    void UploadManager::AddJob(std::unique_ptr<IUploadJob> job)
    {
        jobQueue.emplace_back(std::move(job));
    }

    void UploadManager::AddJobs(std::vector<std::unique_ptr<IUploadJob>> jobs)
    {
        for (auto& job : jobs)
        {
            jobQueue.emplace_back(std::move(job));
        }
    }

    void UploadManager::FlushQueue(const FrameResource& frameResource)
    {
        if (jobQueue.empty())
            return;

        size_t totalSize = GetTotalQueueSize();

        std::shared_ptr<Rendering::Buffer> stagingBuffer = std::make_shared<Rendering::Buffer>(
            Rendering::Buffer::CreateStaging(bufferAllocator, totalSize));

        vk::raii::CommandBuffer copyCommandBuffer = CommandBufferUtilities::BeginSingleTimeCommands(frameResource, device);

        size_t offset = 0;

        for (auto& job : jobQueue)
        {
            job->Execute(stagingBuffer, copyCommandBuffer, offset);
            offset += job->Size;
        }

        Core::CommandBufferUtilities::EndSingleTimeCommands(copyCommandBuffer, device);
        jobQueue.clear();
    }

    size_t UploadManager::GetTotalQueueSize()
    {
        size_t totalSize = 0;

        for (auto& job : jobQueue)
        {
            totalSize += job->Size;
        }

        return totalSize;
    }
} // namespace Beer::Core
