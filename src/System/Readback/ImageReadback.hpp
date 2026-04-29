#pragma once

#include "ImagePixelData.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Readback/IAsyncReadback.hpp"
#include <stdexcept>

namespace Beer::System
{
    class ImageReadback : public IAsyncReadback
    {
        friend class ImageReadbackRequest;

    private:
        Rendering::RenderTexture* texture;
        std::vector<Function<void, ImagePixelData>> callbacks;

    public:
        void Subscribe(Function<void, ImagePixelData> function)
        {
            if (function != nullptr)
            {
                callbacks.push_back(std::move(function));
            }
        }

    private:
        ImageReadback(Rendering::RenderTexture* texture)
            : texture(texture), IAsyncReadback(texture->GetImage()->GetData().GetSize())
        {
        }

        void Execute(Rendering::CommandBuffer* commandBuffer) override
        {
            commandBuffer->CopyImgToBuffer(texture, readbackBuffer.get());
        }

        void Invoke() override
        {
            if (mappedData == nullptr)
            {
                throw std::runtime_error("Image Readback Request mapped data not processed!");
            }

            ImagePixelData pixelData = ImagePixelData(texture->GetImage()->GetData(), mappedData);

            for (auto& function : callbacks)
            {
                if (function != nullptr)
                    function(pixelData);
            }
        }
    };
} // namespace Beer::System
