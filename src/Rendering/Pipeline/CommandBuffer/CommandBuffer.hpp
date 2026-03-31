#pragma once

#include "Core/Application/Renderer/FrameResource.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Mesh/MeshBufferOrder.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderingBeginData.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "System/Components/General/Transform.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/Threads.hpp"

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

        void BindShaderPass(const ShaderPass* shaderPass);
        void BindMaterial(const Material* material);
        void BindMesh(const Mesh* mesh, const MeshBufferOrder* order);
        void BindComputeKernel(const ComputeKernel* compute);
        void BindComputeContext(const ComputeContext* context);

        void DrawMeshSingle(const MeshDrawInfo& info);
        void DrawMeshMultiple(const MeshDrawInfo& info, const uint32_t count);
        void Dispatch(const Threads threads);
    };
} // namespace Beer::Rendering
