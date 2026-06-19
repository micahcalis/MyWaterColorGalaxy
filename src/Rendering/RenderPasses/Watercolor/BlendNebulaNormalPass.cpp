#include "Rendering/RenderPasses/Watercolor/BlendNebulaNormalPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"

namespace Beer::Rendering
{
    static const float ALPHA_THRESHOLD = 0.02f;

    BlendNebulaNormalPass::BlendNebulaNormalPass(NebulaBuffer* nebulaBuffer,
        System::GalaxyObjectBuffer* stardustBuffer)
        : nebulaBuffer(nebulaBuffer)
        , stardustBuffer(stardustBuffer)
        , IRenderPass("BlendNebulaNormalPass", static_cast<uint32_t>(RenderPassEvent::VOLUMETRICS) + 3)
    {
        blendMaterial = std::make_shared<Rendering::Material>("Galaxy/Nebula");
    }

    void BlendNebulaNormalPass::OnRenderSetup(const RenderContext& context)
    {
        nebulaBuffer->ReallocateVolumeAlpha(context);

        blendMaterial->SetTexture("_NebulaVolumeAlpha", nebulaBuffer->NebulaVolumeAlpha);
        blendMaterial->SetFloat("_AlphaThreshold", ALPHA_THRESHOLD);
    }

    void BlendNebulaNormalPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(context.GetMainColorSource(),
            blendMaterial.get(),
            ShaderPassType::DeferredShade,
            context.Output,
            {stardustBuffer->GetPositionOffset()});
    }

    PassDependencyList BlendNebulaNormalPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_NORMAL_OFFSET),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0.5f, 0.5f}),
            static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

        dependencies.AddDependency(PassDependency(NEBULA_VOLUME_ALPHA_NAME,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(NEBULA_VOLUME_ALPHA_FORMAT)));

        return dependencies;
    }
} // namespace Beer::Rendering
