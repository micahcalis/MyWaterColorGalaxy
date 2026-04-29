#pragma once

#include "System/Readback/IAsyncReadback.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include <memory>

namespace Beer::Core
{
    class ReadbackManager;
}

namespace Beer::System
{
    struct IReadbackRequest
    {
        friend class Core::ReadbackManager;

    public:
        virtual ~IReadbackRequest() = default;

    protected:
        virtual std::unique_ptr<IAsyncReadback> CreateReadback() = 0;
    };
} // namespace Beer::System
