#pragma once

#include <vulkan/vulkan.h>
#include "Rendering/Buffer/Buffer.hpp"
#include <memory>

namespace Beer::Core
{
    class IUploadJob
    {
    public:
        VkDeviceSize Size;

    protected:
        void* data;

    public:
        IUploadJob(void* data, VkDeviceSize size)
            : data(data), Size(size)
        {
        }
        virtual ~IUploadJob() = default;

        virtual void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset)
            = 0;
    };
} // namespace Beer::Core
