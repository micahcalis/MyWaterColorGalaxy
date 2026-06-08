#include "Rendering/RenderPasses/Watercolor/ComputeNebulaTiles.hpp"
#include "ComputeNebulaTiles.hpp"
#include "NebulaBuffer.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/RenderPasses/RenderPassEvent.hpp"

namespace Beer::Rendering
{
    static const uint32_t COMPUTE_TILES_GROUPSIZE = 8;
    static const uint32_t COMPUTE_TILES_KERNEL = 0;

    ComputeNebulaTiles::ComputeNebulaTiles(NebulaBuffer* nebulaBuffer,
        System::GalaxyObjectBuffer* stardustBuffer)
        : nebulaBuffer(nebulaBuffer)
        , stardustBuffer(stardustBuffer)
        , IRenderPass("ComputeNebulaTiles", RenderPassEvent::VOLUMETRICS)
    {
    }

    void ComputeNebulaTiles::OnRenderSetup(const RenderContext& context)
    {
        nebulaBuffer->ReallocateTiles(context);

        glm::vec2 tileCount = NebulaBuffer::GetExactTileResolution();
        uint32_t stardustCount = stardustBuffer->GetInstanceCount();

        nebulaBuffer->ComputeTilesContext->SetVector("_TileResolution", glm::vec4(tileCount, 0, 0));
        nebulaBuffer->ComputeTilesContext->SetInt("_StardustCount", stardustCount);

        nebulaBuffer->ComputeTilesContext->SetStructuredBuffer("_DynamicStardustPositions", stardustBuffer->GetPositionBuffer());
        nebulaBuffer->ComputeTilesContext->SetStructuredBuffer("_StardustObjectData", stardustBuffer->GetDataBuffer());
        nebulaBuffer->ComputeTilesContext->SetStructuredBuffer("_NebulaTileBuffer", nebulaBuffer->NebulaTilesBuffer);
        nebulaBuffer->ComputeTilesContext->Update();
    }

    void ComputeNebulaTiles::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        glm::uvec2 tileCount = NebulaBuffer::GetDispatchTileCount();

        Threads threads = Threads::GetCustom((uint32_t)tileCount.x,
            COMPUTE_TILES_GROUPSIZE,
            (uint32_t)tileCount.y,
            COMPUTE_TILES_GROUPSIZE);

        commandBuffer->BindComputeKernel(nebulaBuffer->ComputeTilesContext->GetCompute()->GetKernel(COMPUTE_TILES_KERNEL));
        commandBuffer->BindComputeContext(nebulaBuffer->ComputeTilesContext.get());
        commandBuffer->Dispatch(threads);
    }

    PassDependencyList ComputeNebulaTiles::GetDependencies() const
    {
        PassDependencyList dependenices = PassDependencyList(name);
        dependenices.AddDependency(PassDependency(NEBULA_TILES_NAME,
            ResourceAction::BufferReadWrite));

        return dependenices;
    }

} // namespace Beer::Rendering
