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
        Core::BufferAllocator& allocator;
        BufferAllocation allocation;
        VkDeviceSize size;

    public:
        static Buffer CreateDeviceLocal(Core::BufferAllocator& allocator,
            VkDeviceSize size,
            VkBufferUsageFlags usage);

        static Buffer CreateStaging(Core::BufferAllocator& allocator,
            VkDeviceSize size);

        static Buffer CreateUniform(Core::BufferAllocator& allocator,
            VkDeviceSize size);

        void Upload(const void* data, size_t size);
        void CopyTo(Buffer& dstBuffer,
            const Core::Device& device,
            const Core::FrameResource& frameResource);

        const VkBuffer GetHandle() const { return allocation.Buffer; }

    private:
        Buffer(Core::BufferAllocator& allocator,
            BufferAllocation allocation,
            VkDeviceSize size);
    };
} // namespace Beer::Rendering
