#include "Rendering/RenderPasses/Watercolor/TraceNebulaPass.hpp"
#include "NebulaBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Rendering
{
    static const int32_t MAX_STEPS = 100;
    static const float MAX_LENGTH = 3000.0f;
    static const float STEP_SIZE = 1.0f;

    TraceNebulaPass::TraceNebulaPass(NebulaBuffer* nebulaBuffer,
        System::GalaxyObjectBuffer* stardustBuffer)
        : nebulaBuffer(nebulaBuffer)
        , stardustBuffer(stardustBuffer)
        , IRenderPass("TraceNebulaPass", static_cast<uint32_t>(RenderPassEvent::VOLUMETRICS) + 1)
    {
        traceMaterial = std::make_shared<Rendering::Material>("Galaxy/TraceNebula");
        traceMaterial->SetInt("_MaxRaySteps", MAX_STEPS);
        traceMaterial->SetFloat("_MaxRayLength", MAX_LENGTH);
        traceMaterial->SetFloat("_RayStepSize", STEP_SIZE);
        traceMaterial->SetColor("_TestColor", glm::vec4(1, 0, 0, 1));
    }

    void TraceNebulaPass::OnRenderSetup(const RenderContext& context)
    {
        nebulaBuffer->ReallocateTiles(context);
        nebulaBuffer->ReallocateTarget(context);

        traceMaterial->SetStructuredBuffer("_DynamicStardustPositions", stardustBuffer->GetPositionBuffer());
        traceMaterial->SetStructuredBuffer("_StardustObjectData", stardustBuffer->GetDataBuffer());
        traceMaterial->SetStructuredBuffer("_NebulaTileBuffer", nebulaBuffer->NebulaTilesBuffer);

        glm::uvec2 tileCount = NebulaBuffer::GetDispatchTileCount();
        traceMaterial->SetVector("_TileDimensions", glm::vec4(tileCount.x, tileCount.y, 0, 0));
    }

    void TraceNebulaPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(nullptr,
            traceMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output,
            {stardustBuffer->GetPositionOffset()});
    }

    PassDependencyList TraceNebulaPass::GetDependencies() const
    {
        PassDependencyList dependenices = PassDependencyList(name);
        dependenices.AddDependency(PassDependency(NEBULA_TILES_NAME,
            ResourceAction::BufferRead));

        dependenices.AddDependency(PassDependency(NEBULA_TARGET_NAME,
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(NEBULA_TARGET_FORMAT)));

        return dependenices;
    }

} // namespace Beer::Rendering
