#include "Rendering/RenderPasses/Watercolor/BlendNebulaWCPass.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"

namespace Beer::Rendering
{
    static const float WETNESS = 0.0f;
    static const float GRANULATION_NOISE_INTENSITY = 0.75f;
    static const float COLOR_BLEED = 0.025f;

    BlendNebulaWCPass::BlendNebulaWCPass(NebulaBuffer* nebulaBuffer,
        System::GalaxyObjectBuffer* stardustBuffer)
        : nebulaBuffer(nebulaBuffer)
        , stardustBuffer(stardustBuffer)
        , IRenderPass("TraceNebulaPass", static_cast<uint32_t>(RenderPassEvent::VOLUMETRICS) + 4)
    {
        blendMaterial = std::make_shared<Rendering::Material>("Galaxy/Nebula");
    }

    void BlendNebulaWCPass::OnRenderSetup(const RenderContext& context)
    {
        nebulaBuffer->ReallocateVolumeAlpha(context);

        blendMaterial->SetTexture("_NebulaVolumeAlpha", nebulaBuffer->NebulaVolumeAlpha);
        blendMaterial->SetFloat("_Wetness", WETNESS);
        blendMaterial->SetFloat("_GranulationNoiseIntensity", GRANULATION_NOISE_INTENSITY);
        blendMaterial->SetFloat("_ColorBleed", COLOR_BLEED);
    }

    void BlendNebulaWCPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(context.GetMainColorSource(),
            blendMaterial.get(),
            ShaderPassType::Opaque,
            context.Output,
            {stardustBuffer->GetPositionOffset()});
    }

    PassDependencyList BlendNebulaWCPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(std::string(GBUFFER_WATERCOLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0.0f, 0.0f}),
            static_cast<vk::Format>(GBUFFER_WATERCOLOR_FORMAT)));

        dependencies.AddDependency(PassDependency(NEBULA_VOLUME_ALPHA_NAME,
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(NEBULA_VOLUME_ALPHA_FORMAT)));

        return dependencies;
    }
} // namespace Beer::Rendering
