#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBufferType.hpp"
#include "System/Components/General/Transform.hpp"

namespace Beer::Rendering
{
    class CommandBuffer
    {
    private:
        vk::raii::CommandBuffer commandBuffer;
        CommandBufferType type;

    public:
        void Begin();
        void DrawSingle(Mesh* mesh, Material* material, System::Transform* transform);
        void End();
    };
} // namespace Beer::Rendering
