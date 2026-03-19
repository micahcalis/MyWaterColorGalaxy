#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include "glm/glm.hpp"

namespace Beer::Core
{
    class ImageClearJob : public IUploadJob
    {
    private:
        std::shared_ptr<Rendering::Image> image;
        vk::ClearColorValue clearColor;

    public:
        ImageClearJob(std::shared_ptr<Rendering::Image> image,
            glm::vec4 clearColor)
            : image(image), clearColor(std::array<float, 4>{clearColor.r, clearColor.g, clearColor.b, clearColor.a}), IUploadJob(nullptr, 0)
        {
        }

        void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset) override;
    };
} // namespace Beer::Core
