#include "System/Galaxy/General/GalaxyRenderComponent.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "System/Galaxy/General/GalaxyObjectType.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include <print>
#include <stdexcept>

namespace Beer::System
{
    static const uint32_t OBJECT_TYPE_COUNT = 5;

    static const std::array<GalaxyObjectType, OBJECT_TYPE_COUNT> OBJECT_TYPES = {
        GalaxyObjectType::Planet,
        GalaxyObjectType::Asteroids,
        GalaxyObjectType::SpaceGoo,
        GalaxyObjectType::BlackHole,
        GalaxyObjectType::StarDust};

    BindHistory GalaxyRenderComponent::Bind(BindMask mask,
        Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        for (const auto type : OBJECT_TYPES)
        {
            GalaxyObjectPool* pool = container->GetPool(type);

            if (pool == nullptr)
                continue;

            if (pool->ObjectShader == nullptr || pool->ObjectMesh == nullptr)
            {
                throw std::runtime_error(std::format("Galaxy Object Pool created with null ref: {}!", magic_enum::enum_name(type)));
            }

            if (!pool->ObjectShader->HasPass(pass))
                continue;

            if (pool->Objects.size() == 0)
                continue;

            const Rendering::ShaderPass* shaderPass = pool->ObjectShader->GetPass(pass);
            commandBuffer->BindShaderPass(pool->ObjectShader.get(), shaderPass, renderContext.Output);
            commandBuffer->BindMesh(pool->ObjectMesh.get(), &shaderPass->Input.BufferOrder);

            Rendering::MeshDrawInfo drawInfo = pool->ObjectMesh->GetDrawInfo();

            for (const auto& object : pool->Objects)
            {
                object->Draw(commandBuffer, renderContext, drawInfo);
            }
        }

        return BindHistory(nullptr, nullptr, nullptr);
    }

    bool GalaxyRenderComponent::HasPass(Rendering::ShaderPassType pass) const
    {
        return pass == Rendering::ShaderPassType::Opaque
            || pass == Rendering::ShaderPassType::Transparent;
    }

    BindMask GalaxyRenderComponent::GetBindMask(const BindHistory& history) const
    {
        uint32_t bindMaskBits = 0;
        bindMaskBits |= BIND_SHADER_BITS;
        bindMaskBits |= BIND_MATERIAL_BITS;
        bindMaskBits |= BIND_MESH_BITS;

        return BindMask(bindMaskBits);
    }
} // namespace Beer::System
