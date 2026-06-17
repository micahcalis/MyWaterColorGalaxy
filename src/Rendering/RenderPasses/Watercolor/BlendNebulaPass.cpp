#include "Rendering/RenderPasses/Watercolor/BlendNebulaPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"

namespace Beer::Rendering
{
    BlendNebulaPass::BlendNebulaPass(NebulaBuffer* nebulaBuffer,
        System::GalaxyObjectBuffer* stardustBuffer)
        : nebulaBuffer(nebulaBuffer)
        , stardustBuffer(stardustBuffer)
        , IRenderPass("TraceNebulaPass", static_cast<uint32_t>(RenderPassEvent::VOLUMETRICS) + 2)
    {
        blendMaterial = std::make_shared<Rendering::Material>("Galaxy/Nebula");
    }

    void BlendNebulaPass::OnRenderSetup(const RenderContext& context)
    {
        nebulaBuffer->ReallocateTarget(context);

        blendMaterial->SetTexture("_NebulaColorBuffer", nebulaBuffer->NebulaMarchingTarget);
    }

    void BlendNebulaPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(context.GetMainColorSource(),
            blendMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output,
            {stardustBuffer->GetPositionOffset()});
    }

    PassDependencyList BlendNebulaPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependencies(RenderGlobalUtilities::GetMainColorBlitDependencies());

        dependencies.AddDependency(PassDependency(NEBULA_TARGET_NAME,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(NEBULA_TARGET_FORMAT)));

        return dependencies;
    }

} // namespace Beer::Rendering
