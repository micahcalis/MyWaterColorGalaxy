#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Buffer/BufferAllocation.hpp"
#include "Core/Application/Renderer/BufferAllocator.hpp"
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Beer::Rendering
{
    class Buffer
    {
    private:
        const Core::BufferAllocator& allocator;
        BufferAllocation allocation;
        VkDeviceSize size;

    public:
        static Buffer CreateDeviceLocal(const Core::BufferAllocator& allocator,
            VkDeviceSize size,
            VkBufferUsageFlags usage);

        static Buffer CreateStaging(const Core::BufferAllocator& allocator,
            VkDeviceSize size);

        static Buffer CreateUniform(const Core::BufferAllocator& allocator,
            VkDeviceSize size);

        void Upload(const void* data, size_t size);
        void CopyTo(Buffer& dstBuffer,
            const Core::Device& device,
            const Core::FrameResource& frameResource);

        const VkBuffer GetHandle() const { return allocation.Buffer; }

    private:
        Buffer(const Core::BufferAllocator& allocator,
            BufferAllocation allocation,
            VkDeviceSize size);
    };
} // namespace Beer::Rendering
