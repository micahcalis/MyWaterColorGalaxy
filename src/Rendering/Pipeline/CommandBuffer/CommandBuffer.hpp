#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderingBeginData.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "System/Components/General/Transform.hpp"

namespace Beer::Rendering
{
    class CommandBuffer
    {
    private:
        vk::raii::CommandBuffer commandBuffer = nullptr;

    public:
        CommandBuffer(vk::raii::CommandBuffer commandBuffer);
        void Begin();
        void BeginRendering(const RenderingBeginData& beginData);
        void EndRendering();
        void End();
        void Reset();

        void DrawSingle(Mesh* mesh, Material* material, System::Transform* transform);

        void RecordImageBarrier(vk::PipelineStageFlags srcStage,
            vk::PipelineStageFlags dstStage,
            const vk::ImageMemoryBarrier& barrier);

        void RecordBufferBarrier(vk::PipelineStageFlags srcStage,
            vk::PipelineStageFlags dstStage,
            const vk::BufferMemoryBarrier& barrier);
    };
} // namespace Beer::Rendering
