#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Mesh/MeshDrawInfo.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
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

    void CommandBuffer::BindDescriptorSets(const vk::PipelineBindPoint bindPoint,
        const vk::PipelineLayout layout,
        const uint32_t setIndex,
        std::vector<vk::DescriptorSet> sets)
    {
        commandBuffer.bindDescriptorSets(bindPoint,
            layout,
            setIndex,
            sets,
            nullptr);
    }

    void CommandBuffer::BindModelPush(Rendering::ModelPush modelPush,
        const Rendering::Shader* shader)
    {
        commandBuffer.pushConstants<Rendering::ModelPush>(
            shader->GetPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
            0,
            modelPush);
    }

    void CommandBuffer::BindShaderPass(const ShaderPass* shaderPass)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
            shaderPass->Pipeline);
    }

    void CommandBuffer::BindMaterial(const Material* material)
    {
        BindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            material->GetShader()->GetPipelineLayout(),
            MaterialData::SET,
            {material->GetDescriptorSet()});
    }

    void CommandBuffer::BindMesh(const Mesh* mesh, const MeshBufferOrder* bufferOrder)
    {
        std::vector<vk::Buffer> activeBuffers;
        const Rendering::MeshBuffers& buffers = mesh->GetBuffers();

        for (int i = 0; i < bufferOrder->GetSize(); i++)
        {
            const Rendering::MeshBufferType type = bufferOrder->GetElement(i);

            if (buffers.HasBuffer(type))
                activeBuffers.push_back(buffers.GetBuffer(type)->GetHandle());
        }

        std::vector<vk::DeviceSize> offsets(activeBuffers.size(), 0);

        commandBuffer.bindVertexBuffers(0, activeBuffers, offsets);

        if (!buffers.HasIndex())
            return;

        commandBuffer.bindIndexBuffer(buffers.IndexBuffer->GetHandle(), 0, vk::IndexType::eUint32);
    }

    void CommandBuffer::DrawMeshSingle(const MeshDrawInfo& info)
    {
        if (info.CanIndex())
        {
            commandBuffer.drawIndexed(info.GetIndexCount(),
                1,
                0,
                0,
                0);
        } else
        {
            commandBuffer.draw(info.GetVertexCount(),
                1,
                0,
                0);
        }
    }
} // namespace Beer::Rendering
