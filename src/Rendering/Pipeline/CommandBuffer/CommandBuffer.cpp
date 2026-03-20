#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "vulkan/vulkan.hpp"

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

    void CommandBuffer::BeginRendering(const RenderingBeginData& beginData)
    {
        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = vk::Rect2D({0, 0}, vk::Extent2D(beginData.Width, beginData.Height));
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = beginData.ColorWriteTargets.size();
        renderingInfo.pColorAttachments = beginData.ColorWriteTargets.data();

        if (beginData.WritesToDepth)
        {
            renderingInfo.pDepthAttachment = &beginData.DepthWriteTarget;
        }

        commandBuffer.beginRendering(renderingInfo);

        commandBuffer.setViewport(0,
            vk::Viewport(0.0f,
                0.0f,
                static_cast<float>(beginData.Width),
                static_cast<float>(beginData.Height),
                0.0f,
                1.0f));

        commandBuffer.setScissor(0, renderingInfo.renderArea);
    }

    void CommandBuffer::EndRendering()
    {
        commandBuffer.endRendering();
    }

    void CommandBuffer::End()
    {
        commandBuffer.end();
    }

    void CommandBuffer::Reset()
    {
        commandBuffer.reset();
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
