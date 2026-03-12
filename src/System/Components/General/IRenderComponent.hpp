#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Drawing/BindHistory.hpp"
#include "System/Drawing/BindMask.hpp"
#include "System/Drawing/Layer.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::System
{
    class IRenderComponent
    {
    public:
        virtual ~IRenderComponent() = default;

        virtual BindHistory Bind(BindMask mask, vk::CommandBuffer commandBuffer, const Rendering::ShaderPassType pass) = 0;

        virtual bool HasPass(Rendering::ShaderPassType pass) const = 0;
        virtual Layer GetLayer() const = 0;
        virtual BindMask GetBindMask(const BindHistory& history) const = 0;

        virtual const Rendering::Shader* GetPrimaryShader() const = 0;
        virtual const Rendering::Material* GetPrimaryMaterial() const = 0;
        virtual const Rendering::Mesh* GetPrimaryMesh() const = 0;
    };
} // namespace Beer::System
