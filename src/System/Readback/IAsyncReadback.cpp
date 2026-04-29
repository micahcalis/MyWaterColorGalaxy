#include "System/Readback/IAsyncReadback.hpp"
#include "Core/Application/Managers/ReadbackManager.hpp"
#include <stdexcept>

namespace Beer::System
{
    IAsyncReadback* IAsyncReadback::Get(std::unique_ptr<IReadbackRequest> request)
    {
        if (readbackManager == nullptr)
        {
            throw std::runtime_error("Readback Manager is unitialized");
        }

        return readbackManager->CreateReadback(std::move(request));
    }
} // namespace Beer::System
