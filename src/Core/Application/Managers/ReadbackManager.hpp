#pragma once

#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/FrameResource.hpp"
#include "Core/Application/Renderer/TimelineSemaphore.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "System/Readback/IReadbackRequest.hpp"
#include "System/Readback/IAsyncReadback.hpp"
#include <memory>

namespace Beer::Core
{
    class ReadbackManager
    {
    private:
        const Device& device;
        std::vector<std::unique_ptr<System::IAsyncReadback>> readbackQueue;
        std::unique_ptr<TimelineSemaphore> timelineSemaphore = nullptr;

    public:
        ReadbackManager(const Device& device)
            : device(device)
        {
            timelineSemaphore = std::make_unique<TimelineSemaphore>(device);
        }

        System::IAsyncReadback* CreateReadback(std::unique_ptr<System::IReadbackRequest> readbackRequest);
        void Update(const FrameResource& frameResource);

    private:
        void CheckWaiting() const;
        void Cleanup();
        bool HasNewWork() const;
    };
} // namespace Beer::Core
