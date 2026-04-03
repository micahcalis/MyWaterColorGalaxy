#include "Rendering/RenderPasses/ComputeTornadoParticlesPass.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Compute/Threads.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"

namespace Beer::Rendering
{
    ComputeTornadoParticlesPass::ComputeTornadoParticlesPass(const uint32_t intEvent)
        : IRenderPass("Compute Tornado", intEvent)
    {
        std::shared_ptr<Rendering::ComputeShader> computeTest = Rendering::ComputeShader::Get("TestInstancing");
        instanceTestCompContext = std::make_shared<Rendering::ComputeContext>(computeTest);

        instanceTestCompContext->SetInt("_ParticleCount", TORNADO_PART_COUNT);
        instanceTestCompContext->SetFloat("_Speed", 5.0f);
        instanceTestCompContext->SetFloat("_MaxRadius", 8.0f);
        instanceTestCompContext->Update();
    }

    void ComputeTornadoParticlesPass::OnRenderSetup(const RenderContext& context)
    {
        context.BlackBox->ReallocateIfNeeded(TORNADO_BUFFER_NAME,
            PhaseBuffer::CalculateSize(TORNADO_PART_COUNT, sizeof(glm::vec4)));

        particleBuffer = context.BlackBox->GetResource<PhaseBuffer>(TORNADO_BUFFER_NAME);
        instanceTestCompContext->SetStructuredBuffer("_ParticleBuffer", particleBuffer);
        instanceTestCompContext->Update();
    }

    void ComputeTornadoParticlesPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::Get1D(TORNADO_PART_COUNT, 32);
        commandBuffer->BindComputeKernel(instanceTestCompContext->GetCompute()->GetKernel(0));
        commandBuffer->BindComputeContext(instanceTestCompContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList ComputeTornadoParticlesPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(TORNADO_BUFFER_NAME,
            ResourceAction::ComputeBufferReadWrite));

        return dependencies;
    }
} // namespace Beer::Rendering
