#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"

namespace Beer::Rendering
{
    CommandBuffer::CommandBuffer(vk::raii::CommandBuffer commandBuffer)
        : commandBuffer(std::move(commandBuffer))
    {
    }

    void CommandBuffer::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffer.begin(beginInfo);
    }

    void CommandBuffer::Reset()
    {
        commandBuffer.reset();
    }

    void CommandBuffer::End()
    {
        commandBuffer.end();
    }

    void CommandBuffer::DrawSingle(Mesh* mesh, Material* material, System::Transform* transform)
    {
    }

    void CommandBuffer::RecordImageBarrier(vk::PipelineStageFlags srcStage,
        vk::PipelineStageFlags dstStage,
        const vk::ImageMemoryBarrier& barrier)
    {
        commandBuffer.pipelineBarrier(
            srcStage,
            dstStage,
            vk::DependencyFlags(),
            nullptr,
            nullptr,
            barrier);
    }

    void CommandBuffer::RecordBufferBarrier(vk::PipelineStageFlags srcStage,
        vk::PipelineStageFlags dstStage,
        const vk::BufferMemoryBarrier& barrier)
    {
        commandBuffer.pipelineBarrier(
            srcStage,
            dstStage,
            vk::DependencyFlags(),
            nullptr,
            barrier,
            nullptr);
    }

} // namespace Beer::Rendering
