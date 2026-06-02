#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include "System/Delegates/Delegate.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Core
{
    class ImageCopyJob : public IUploadJob
    {
    private:
        std::shared_ptr<Rendering::Image> sourceImage = nullptr;
        std::shared_ptr<Rendering::Image> destinationImage = nullptr;
        vk::ImageLayout sourceLayout;

    public:
        ImageCopyJob(std::shared_ptr<Rendering::Image> sourceImage,
            std::shared_ptr<Rendering::Image> destinationImage,
            vk::ImageLayout sourceLayout = vk::ImageLayout::eShaderReadOnlyOptimal)
            : sourceImage(sourceImage)
            , destinationImage(destinationImage)
            , sourceLayout(sourceLayout)
            , IUploadJob(nullptr, 0)
        {
        }

        void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset) override;
    };
} // namespace Beer::Core
