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
        void EndRendering();
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

        void DrawMeshSingle(const MeshDrawInfo& info);
    };
} // namespace Beer::Rendering
