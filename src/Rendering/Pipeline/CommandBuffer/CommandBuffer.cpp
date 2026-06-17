#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Renderer/TimelineSemaphore.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Mesh/MeshDrawInfo.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderingBeginData.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ISyncBarrier.hpp"
#include "Rendering/Pipeline/Frame/Synchronization/ImageSyncBarrier.hpp"
#include "Rendering/Shader/FragmentOutput.hpp"
#include "Rendering/Shader/Globals/ModelTransformData.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Components/General/Transform.hpp"
#include "vulkan/vulkan.hpp"
#include <stdexcept>

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
        if (!beginData.IsDrawPass)
            return;

        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea = vk::Rect2D({0, 0}, vk::Extent2D(beginData.Width, beginData.Height));
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = beginData.ColorWriteTargets.size();
        renderingInfo.pColorAttachments = beginData.ColorWriteTargets.data();

        if (beginData.TestsDepth)
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

    void CommandBuffer::EndRendering(const bool isDrawPass)
    {
        if (!isDrawPass)
            return;

        commandBuffer.endRendering();
    }

    void CommandBuffer::End()
    {
        commandBuffer.end();
    }

    void CommandBuffer::EndAsync(const Core::Device& device, Core::TimelineSemaphore* timelineSemaphore)
    {
        commandBuffer.end();

        vk::TimelineSemaphoreSubmitInfo timelineInfo = timelineSemaphore->GetSubmitInfo();
        vk::Semaphore signalSemaphore = timelineSemaphore->GetHandle();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;
        submitInfo.pNext = &timelineInfo;

        device.GetGraphicsQueue().submit(submitInfo, nullptr);
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
        std::vector<vk::DescriptorSet> sets,
        const std::vector<uint32_t>& dynamicOffsets)
    {
        commandBuffer.bindDescriptorSets(bindPoint,
            layout,
            setIndex,
            sets,
            dynamicOffsets);
    }

    void CommandBuffer::BindModelPush(Rendering::ModelPush modelPush,
        const Rendering::Shader* shader)
    {
        commandBuffer.pushConstants<Rendering::ModelPush>(
            shader->GetPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute,
            0,
            modelPush);
    }

    void CommandBuffer::BindRectPush(Rendering::RectPush rectPush,
        const Rendering::Shader* shader)
    {
        commandBuffer.pushConstants<Rendering::RectPush>(
            shader->GetPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute,
            0,
            rectPush);
    }

    void CommandBuffer::BindInstancingPush(const Rendering::Shader* shader)
    {
        commandBuffer.pushConstants<Rendering::ModelPush>(
            shader->GetPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute,
            0,
            ModelPush(0, 0, true));
    }

    void CommandBuffer::BindInstancingTransforms(const std::vector<System::Transform>& transforms,
        const RenderContext& context,
        const Rendering::Shader* shader)
    {
        BindInstancingPush(shader);

        void* mappedTransformData = context.TransformBuffer->GetMappedPointer();

        memcpy(mappedTransformData,
            System::Transform::ToModelData(transforms).data(),
            sizeof(ModelTransformData) * transforms.size());
    }

    void CommandBuffer::BindInstancingTransforms(const std::vector<ModelTransformData>& modelData,
        const RenderContext& context,
        const Rendering::Shader* shader)
    {
        BindInstancingPush(shader);

        void* mappedTransformData = context.TransformBuffer->GetMappedPointer();

        memcpy(mappedTransformData,
            modelData.data(),
            sizeof(ModelTransformData) * modelData.size());
    }

    void CommandBuffer::BindShaderPass(const Shader* shader,
        const ShaderPass* shaderPass,
        const FragmentOutput& output)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
            shader->GetPipeline(shaderPass, output));
    }

    void CommandBuffer::BindMaterial(const Material* material,
        const std::vector<uint32_t>& dynamicOffsets)
    {
        BindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            material->GetShader()->GetPipelineLayout(),
            MaterialData::SET,
            {material->GetDescriptorSet()},
            dynamicOffsets);
    }

    void CommandBuffer::BindFontMaterial(const FontMaterial* fontMaterial)
    {
        BindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            fontMaterial->GetShader()->GetPipelineLayout(),
            MaterialData::SET,
            {fontMaterial->GetDescriptorSet()});
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

    void CommandBuffer::BindTextBuffer(const TextBuffer* textBuffer)
    {
        vk::Buffer buffers[] = {
            textBuffer->GetPosBuffer()->GetHandle(),
            textBuffer->GetUVBuffer()->GetHandle()};
        VkDeviceSize offsets[] = {0, 0};

        commandBuffer.bindVertexBuffers(0, buffers, offsets);
        commandBuffer.bindIndexBuffer(textBuffer->GetIndexBuffer()->GetHandle(), 0, vk::IndexType::eUint32);
    }

    void CommandBuffer::BindQuadBuffer(const QuadBuffer* quadBuffer)
    {
        vk::Buffer buffers[] = {
            quadBuffer->GetPosBuffer()->GetHandle(),
            quadBuffer->GetUVBuffer()->GetHandle()};
        VkDeviceSize offsets[] = {0, 0};

        commandBuffer.bindVertexBuffers(0, buffers, offsets);
        commandBuffer.bindIndexBuffer(quadBuffer->GetIndexBuffer()->GetHandle(), 0, vk::IndexType::eUint32);
    }

    void CommandBuffer::BindComputeKernel(const ComputeKernel* kernel)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, kernel->Pipeline);
    }

    void CommandBuffer::BindComputeContext(const ComputeContext* context,
        const std::vector<uint32_t>& dynamicOffsets)
    {
        BindDescriptorSets(vk::PipelineBindPoint::eCompute,
            context->GetCompute()->GetPipelineLayout(),
            MaterialData::SET,
            {context->GetDescriptorSet()},
            dynamicOffsets);
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

    void CommandBuffer::DrawMeshMultiple(const MeshDrawInfo& info, const uint32_t count)
    {
        if (info.CanIndex())
        {
            commandBuffer.drawIndexed(info.GetIndexCount(),
                count,
                0,
                0,
                0);
        } else
        {
            commandBuffer.draw(info.GetVertexCount(),
                count,
                0,
                0);
        }
    }

    void CommandBuffer::DrawIndexedSlice(const uint32_t firstIndex, const uint32_t indexCount, const uint32_t instanceCount)
    {
        commandBuffer.drawIndexed(indexCount,
            instanceCount,
            firstIndex,
            0,
            0);
    }

    void CommandBuffer::Dispatch(const Threads threads)
    {
        if (threads.X == 0 || threads.Y == 0 || threads.Z == 0)
            throw std::runtime_error("Can't Dispatch Compute With 0 Thread Groups");

        commandBuffer.dispatch(threads.X, threads.Y, threads.Z);
    }

    void CommandBuffer::Blit(RenderTexture* source,
        Material* material,
        const ShaderPassType pass,
        const FragmentOutput& output,
        const std::vector<uint32_t>& dynamicOffsets)
    {
        const Rendering::Shader* shader = material->GetShader();
        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        if (source != nullptr)
        {
            material->SetTexture("_BlitSource", source);
        }

        BindShaderPass(shader, shaderPass, output);
        BindMaterial(material, dynamicOffsets);

        commandBuffer.draw(3, 1, 0, 0);
    }

    void CommandBuffer::CopyImgToBuffer(Rendering::RenderTexture* texture, Rendering::Buffer* buffer)
    {
        ResourceAction previousAction = texture->GetCurrentAction();
        std::unique_ptr<ISyncBarrier> barrier = texture->GetBarrier(ResourceAction::TransferRead);
        barrier->RecordBarrier(this);

        vk::BufferImageCopy copyRegion{};
        copyRegion.bufferOffset = 0;
        copyRegion.bufferRowLength = 0;
        copyRegion.bufferImageHeight = 0;

        ImageData data = texture->GetImage()->GetData();

        copyRegion.imageSubresource.aspectMask = data.AspectMask;
        copyRegion.imageSubresource.mipLevel = data.BaseMipLevel;
        copyRegion.imageSubresource.baseArrayLayer = data.BaseArrayLayer;
        copyRegion.imageSubresource.layerCount = data.ArrayLayers;
        copyRegion.imageOffset = vk::Offset3D{0, 0, 0};
        copyRegion.imageExtent = data.Extent;

        commandBuffer.copyImageToBuffer(texture->GetImage()->GetHandle(),
            vk::ImageLayout::eTransferSrcOptimal,
            buffer->GetHandle(),
            copyRegion);

        std::unique_ptr<ISyncBarrier> resetBarrier = texture->GetBarrier(previousAction);
        resetBarrier->RecordBarrier(this);
    }
} // namespace Beer::Rendering
