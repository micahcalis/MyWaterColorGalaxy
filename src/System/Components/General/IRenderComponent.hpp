#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Drawing/BindHistory.hpp"
#include "System/Drawing/BindMask.hpp"
#include "System/Drawing/ShaderPassMask.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::System
{
    class IRenderComponent
    {
    public:
        virtual ~IRenderComponent() = default;
        virtual BindHistory Bind(BindMask mask, vk::CommandBuffer commandBuffer, const Rendering::ShaderPassType pass) = 0;
        virtual bool HasPass(ShaderPassMask mask) = 0;
        virtual BindMask GetBindMask(BindHistory history) = 0;

        virtual const Rendering::Shader* GetPrimaryShader() = 0;
        virtual const Rendering::Material* GetPrimaryMaterial() = 0;
        virtual const Rendering::Mesh* GetPrimaryMesh() = 0;
    };
} // namespace Beer::System
