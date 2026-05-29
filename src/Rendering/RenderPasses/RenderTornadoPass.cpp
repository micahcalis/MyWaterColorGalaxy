#include "Rendering/RenderPasses/RenderTornadoPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "RenderGlobalSettings.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/ComputeTornadoParticlesPass.hpp"
#include <memory>

namespace Beer::Rendering
{
    RenderTornadoPass::RenderTornadoPass(const uint32_t intEvent)
        : IRenderPass("Render Tornado", intEvent)
    {
        particleMaterial = std::make_shared<Material>("TornadoParticle");
        cubeMesh = Mesh::Get("MDL_IcoSphere");
    }

    void RenderTornadoPass::OnRenderSetup(const RenderContext& context)
    {
        context.BlackBox->ReallocateIfNeeded(TORNADO_BUFFER_NAME,
            PhaseBuffer::CalculateSize(TORNADO_PART_COUNT, sizeof(glm::vec4)));

        particleBuffer = context.BlackBox->GetResource<PhaseBuffer>(TORNADO_BUFFER_NAME);
        particleMaterial->SetStructuredBuffer("_ParticleBuffer", particleBuffer);
    }

    void RenderTornadoPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        const ShaderPass* pass = particleMaterial->GetShader()->GetPass(ShaderPassType::Opaque);
        commandBuffer->BindShaderPass(particleMaterial->GetShader(), pass, context.Output);
        commandBuffer->BindMaterial(particleMaterial.get());
        commandBuffer->BindMesh(cubeMesh.get(), &pass->Input.BufferOrder);

        Rendering::MeshDrawInfo drawInfo = cubeMesh->GetDrawInfo();
        commandBuffer->DrawMeshMultiple(drawInfo, TORNADO_PART_COUNT);
    }

    PassDependencyList RenderTornadoPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0, 0, 0, 0}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(std::string(MAIN_DEPTH),
            ResourceAction::DepthWrite,
            ResetOperator::ClearDepth()));

        dependencies.AddDependency(PassDependency(TORNADO_BUFFER_NAME,
            ResourceAction::BufferRead));

        return dependencies;
    }
} // namespace Beer::Rendering
