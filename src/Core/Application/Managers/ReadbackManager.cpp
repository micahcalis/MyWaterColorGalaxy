#include "Core/Application/Managers/ReadbackManager.hpp"
#include "Core/Application/Utilities/CommandBufferUtilities.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "System/Readback/IAsyncReadback.hpp"
#include "System/Readback/IReadbackRequest.hpp"

namespace Beer::Core
{
    System::IAsyncReadback* ReadbackManager::CreateReadback(std::unique_ptr<System::IReadbackRequest> readbackRequest)
    {
        std::unique_ptr<System::IAsyncReadback> readback = readbackRequest->CreateReadback();
        System::IAsyncReadback* readbackP = readback.get();

        readbackQueue.push_back(std::move(readback));
        return readbackP;
    }

    void ReadbackManager::Update(const FrameResource& frameResource)
    {
        if (readbackQueue.empty())
            return;

        CheckWaiting();
        Cleanup();

        if (!HasNewWork())
            return;

        std::shared_ptr<Rendering::CommandBuffer> commandBuffer = std::make_shared<Rendering::CommandBuffer>((
            CommandBufferUtilities::BeginSingleTimeCommands(frameResource, device)));

        uint64_t batchTicket = timelineSemaphore->AssignTicket();

        for (auto& readback : readbackQueue)
        {
            System::RequestState state = readback->GetState();

            if (state == System::RequestState::Uninitialized)
            {
                readback->Execute(commandBuffer.get());
                readback->SetTicket(batchTicket);
                readback->SetState(System::RequestState::Waiting);
                readback->SetCommandBuffer(commandBuffer);
            }
        }

        commandBuffer->EndAsync(device, timelineSemaphore.get());
    }

    void ReadbackManager::CheckWaiting() const
    {
        uint64_t signValue = timelineSemaphore->GetCurrentSignValue(device);

        for (auto& readback : readbackQueue)
        {
            System::RequestState state = readback->GetState();

            if (state == System::RequestState::Waiting)
            {
                if (readback->GetTicket() > signValue)
                    continue;

                readback->Process();
                readback->Invoke();
                readback->SetState(System::RequestState::Ready);
            }
        }
    }

    void ReadbackManager::Cleanup()
    {
        std::erase_if(readbackQueue, [](const auto& readback) {
            return readback->GetState() == System::RequestState::Ready;
        });
    }

    bool ReadbackManager::HasNewWork() const
    {
        bool hasNewWork = false;
        for (const auto& readback : readbackQueue)
        {
            if (readback->GetState() == System::RequestState::Uninitialized)
            {
                hasNewWork = true;
                break;
            }
        }

        return hasNewWork;
    }
} // namespace Beer::Core
