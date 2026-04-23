#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    WaterColorSimBuffers::WaterColorSimBuffers(Material* debugMaterial)
        : debugMaterial(debugMaterial)
    {
        SimulationContext = std::make_shared<ComputeContext>("Painting/InteractivePainting");
    }

    void WaterColorSimBuffers::ReallocateCanvas(const RenderContext& context)
    {
        CanvasBuffer = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(CANVAS_BUFFER,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(CANVAS_BUFFER_FORMAT),
                                TextureAccess::ReadWrite)
                .AllocPointer);
    }

    void WaterColorSimBuffers::ReallocateWater(const RenderContext& context)
    {
        ShallowWater = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(SHALLOW_WATER,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(SHALLOW_WATER_FORMAT),
                                TextureAccess::ReadWrite)
                .AllocPointer);
    }

    void WaterColorSimBuffers::ReallocateFlux(const RenderContext& context)
    {
        FluxBuffer = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(FLUX_BUFFER,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(FLUX_BUFFER_FORMAT),
                                TextureAccess::ReadWrite)
                .AllocPointer);
    }
} // namespace Beer::Rendering
