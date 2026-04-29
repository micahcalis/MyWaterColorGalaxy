#include "System/Readback/ImageReadbackRequest.hpp"
#include "Core/Application/Utilities/ImageUtilities.hpp"
#include "IReadbackRequest.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Readback/ImageReadback.hpp"
#include <memory>

namespace Beer::System
{
    ImageReadbackRequest::ImageReadbackRequest(Rendering::RenderTexture* texture)
        : texture(texture)
    {
    }

    std::unique_ptr<IAsyncReadback> ImageReadbackRequest::CreateReadback()
    {
        return std::unique_ptr<IAsyncReadback>(new ImageReadback(texture));
    }
} // namespace Beer::System
