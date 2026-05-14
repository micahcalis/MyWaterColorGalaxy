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
        container->Draw(commandBuffer,
            renderContext,
            pass);

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
