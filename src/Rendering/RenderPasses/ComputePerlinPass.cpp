#include "Rendering/RenderPasses/ComputePerlinPass.hpp"
#include "ComputePerlinPass.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>

namespace Beer::Rendering
{
    ComputePerlinPass::ComputePerlinPass(uint32_t intEvent, Material* perlinViewMat)
        : perlinViewMat(perlinViewMat), IRenderPass("Compute Perlin", intEvent)
    {
        std::shared_ptr<Rendering::ComputeShader> computeTest = Rendering::ComputeShader::Get("TestCompute");
        computeTest->PrintConfig();
        perlinCompContext = std::make_shared<Rendering::ComputeContext>(computeTest);

        perlinCompContext->SetFloat("_Tiling", 4.0f);
        perlinCompContext->SetFloat("_NoiseIntensity", 1.0f);
        perlinCompContext->SetFloat("_NoiseExponent", 1.0f);
        perlinCompContext->SetInt("_Resolution", PERLIN_TEX_RES);
        perlinCompContext->Update();
    }

    void ComputePerlinPass::OnRenderSetup(const RenderContext& context)
    {
        context.BlackBox->ReallocateIfNeeded(PERLIN_TEX_NAME,
            PERLIN_TEX_RES,
            PERLIN_TEX_RES,
            static_cast<VkFormat>(vk::Format::eR8G8B8A8Unorm));

        RenderTexture* noiseTex = context.BlackBox->GetResource<RenderTexture>(PERLIN_TEX_NAME);
        perlinCompContext->SetTexture("_NoiseTex", noiseTex);
        perlinViewMat->SetTexture("_AlbedoMap", noiseTex);

        Threads threads = Threads::GetProportional2D(PERLIN_TEX_RES, 8);
        perlinCompContext->SetInt("_ThreadGroups", threads.X);
        perlinCompContext->Update();
    }

    void ComputePerlinPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        Threads threads = Threads::GetProportional2D(PERLIN_TEX_RES, 8);
        commandBuffer->BindComputeKernel(perlinCompContext->GetCompute()->GetKernel(1));
        commandBuffer->BindComputeContext(perlinCompContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList ComputePerlinPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);
        dependencies.AddDependency(PassDependency(PERLIN_TEX_NAME,
            ResourceAction::ComputeReadWrite));

        return dependencies;
    }
} // namespace Beer::Rendering
