#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "Core/Application/Renderer/TimelineSemaphore.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Mesh/MeshBufferOrder.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderingBeginData.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "Rendering/Texture/ITexture.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "System/Components/General/Transform.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/Threads.hpp"
#include "Rendering/Quads/QuadBuffer.hpp"

namespace Beer::Rendering
{
    struct RenderContext;
}

namespace Beer::Rendering
{
    class CommandBuffer
    {
        friend class Core::FrameResource;

    private:
        vk::raii::CommandBuffer commandBuffer = nullptr;

    public:
        CommandBuffer(vk::raii::CommandBuffer commandBuffer);
        vk::CommandBuffer GetVk() const { return *commandBuffer; }
        void Begin();
        void BeginRendering(const RenderingBeginData& beginData);
        void EndRendering(const bool isDrawPass);
        void End();
        void EndAsync(const Core::Device& device, Core::TimelineSemaphore* timelineSemaphore);
        void Reset();

        void RecordImageBarrier(vk::PipelineStageFlags srcStage,
            vk::PipelineStageFlags dstStage,
            const vk::ImageMemoryBarrier& barrier);

        void RecordBufferBarrier(vk::PipelineStageFlags srcStage,
            vk::PipelineStageFlags dstStage,
            const vk::BufferMemoryBarrier& barrier);

        void BindDescriptorSets(const vk::PipelineBindPoint bindPoint,
            const vk::PipelineLayout layout,
            const uint32_t setIndex,
            std::vector<vk::DescriptorSet> sets);

        void BindModelPush(Rendering::ModelPush modelPush,
            const Rendering::Shader* shader);

        void BindRectPush(Rendering::RectPush rectPush,
            const Rendering::Shader* shader);

        void BindInstancingPush(const Rendering::Shader* shader);

        void BindInstancingTransforms(const std::vector<System::Transform>& transforms,
            const RenderContext& context,
            const Rendering::Shader* shader);

        void BindInstancingTransforms(const std::vector<ModelTransformData>& modelData,
            const RenderContext& context,
            const Rendering::Shader* shader);

        void BindShaderPass(const Shader* shader,
            const ShaderPass* shaderPass,
            const FragmentOutput& output);

        void BindMaterial(const Material* material);
        void BindFontMaterial(const FontMaterial* fontMaterial);
        void BindMesh(const Mesh* mesh, const MeshBufferOrder* order);
        void BindTextBuffer(const TextBuffer* textBuffer);
        void BindQuadBuffer(const QuadBuffer* quadBuffer);
        void BindComputeKernel(const ComputeKernel* compute);
        void BindComputeContext(const ComputeContext* context);

        void DrawMeshSingle(const MeshDrawInfo& info);
        void DrawMeshMultiple(const MeshDrawInfo& info, const uint32_t count);
        void DrawIndexedSlice(const uint32_t firstIndex, const uint32_t indexCount, const uint32_t instanceCount = 1);
        void Dispatch(const Threads threads);

        void Blit(RenderTexture* source,
            Material* material,
            const ShaderPassType pass,
            const FragmentOutput& output);

        void CopyImgToBuffer(Rendering::RenderTexture* texture, Rendering::Buffer* buffer);
    };
} // namespace Beer::Rendering
