#pragma once

#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "System/Delegates/Delegate.hpp"
#include <cstdint>
#include <memory>

namespace Beer::Core
{
    class ReadbackManager;
}

namespace Beer::System
{
    struct IReadbackRequest;

    enum class RequestState
    {
        Uninitialized,
        Waiting,
        Ready
    };

    class IAsyncReadback
    {
        friend class Core::ReadbackManager;

    protected:
        std::shared_ptr<Rendering::Buffer> readbackBuffer;
        std::shared_ptr<Rendering::CommandBuffer> commandBuffer = nullptr;
        RequestState state = RequestState::Uninitialized;
        uint64_t ticket = 0;
        void* mappedData = nullptr;

    public:
        virtual ~IAsyncReadback() = default;

        IAsyncReadback(size_t size)
        {
            readbackBuffer = std::make_shared<Rendering::Buffer>(Rendering::Buffer::CreateReadback(size));
        }

        RequestState GetState() const
        {
            return state;
        }

        uint64_t GetTicket() const { return ticket; }

    protected:
        virtual void Execute(Rendering::CommandBuffer* commandBuffer) = 0;

        void Process()
        {
            mappedData = readbackBuffer->GetAllocInfo().pMappedData;
            readbackBuffer->Invalidate();
        }

        void SetState(RequestState state)
        {
            this->state = state;
        }

        void SetTicket(uint64_t ticket)
        {
            this->ticket = ticket;
        }

        void SetCommandBuffer(std::shared_ptr<Rendering::CommandBuffer> commandBuffer)
        {
            this->commandBuffer = commandBuffer;
        }

        virtual void Invoke() = 0;

    private:
        inline static Core::ReadbackManager* readbackManager = nullptr;

    public:
        static void SetReadbackManager(Core::ReadbackManager* readbackManager)
        {
            IAsyncReadback::readbackManager = readbackManager;
        }

        static IAsyncReadback* Get(std::unique_ptr<IReadbackRequest> request);
    };
} // namespace Beer::System
