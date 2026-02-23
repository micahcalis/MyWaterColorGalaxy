#include "Core/Application/Jobs/ImageUploadJob.hpp"

namespace Beer::Core
{
    ImageUploadJob::~ImageUploadJob()
    {
        if (imageAsset.Pixels)
        {
            stbi_image_free(imageAsset.Pixels);
        }
    }

    void ImageUploadJob::Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
        vk::raii::CommandBuffer& commandBuffer,
        const size_t offset)
    {
        stagingBuffer->Upload(data, Size, offset);
        stagingBuffer->QueueStagingTransfer(*image, commandBuffer, Size, offset);
    }
} // namespace Beer::Core
