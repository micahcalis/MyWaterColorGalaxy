#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include "Core/Assets/ImageAsset.hpp"
#include "IUploadJob.hpp"
#include "Rendering/Buffer/Image.hpp"
#include <memory>

namespace Beer::Core
{
    class ImageUploadJob : public IUploadJob
    {
    private:
        std::shared_ptr<Rendering::Image> image;
        ImageAsset imageAsset;

    public:
        ImageUploadJob(std::shared_ptr<Rendering::Image> image,
            ImageAsset imageAsset)
            : image(image), imageAsset(imageAsset), IUploadJob(imageAsset.Pixels, imageAsset.Size)
        {
        }

        ~ImageUploadJob() override;

        void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset) override;
    };
} // namespace Beer::Core
