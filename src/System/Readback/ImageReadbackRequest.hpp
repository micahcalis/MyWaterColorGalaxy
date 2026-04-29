#pragma once

#include "IReadbackRequest.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Texture/RenderTexture.hpp"

namespace Beer::System
{
    struct ImageReadbackRequest : public IReadbackRequest
    {
    private:
        Rendering::RenderTexture* texture;

    public:
        ImageReadbackRequest(Rendering::RenderTexture* texture);
        std::unique_ptr<IAsyncReadback> CreateReadback() override;
    };
} // namespace Beer::System
