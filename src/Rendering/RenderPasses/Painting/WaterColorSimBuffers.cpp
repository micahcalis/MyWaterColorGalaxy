#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "WaterColorSimBuffers.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    WaterColorSimBuffers::WaterColorSimBuffers(Material* debugMaterial)
        : debugMaterial(debugMaterial)
    {
        CanvasBaseTexture = std::make_shared<Texture2D>(Image::GetAsset(CANVAS_BASE_NAME));
        SimulationContext = std::make_shared<ComputeContext>("Painting/InteractivePainting");
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

    void WaterColorSimBuffers::ReallocateSuspended(const RenderContext& context)
    {
        SuspendedPigmentPongA = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(SUSPENDED_PIGMENT_A,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(SUSPENDED_PIGMENT_FORMAT),
                                TextureAccess::ReadWrite,
                                vk::Filter::eLinear,
                                vk::SamplerAddressMode::eRepeat,
                                glm::vec4(0),
                                PIGMENT_LAYERS)
                .AllocPointer);

        SuspendedPigmentPongB = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(SUSPENDED_PIGMENT_B,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(SUSPENDED_PIGMENT_FORMAT),
                                TextureAccess::ReadWrite,
                                vk::Filter::eLinear,
                                vk::SamplerAddressMode::eRepeat,
                                glm::vec4(0),
                                PIGMENT_LAYERS)
                .AllocPointer);
    }

    void WaterColorSimBuffers::ReallocateDeposited(const RenderContext& context)
    {
        DepositedPigment = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(DEPOSITED_PIGMENT,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(DEPOSITED_PIGMENT_FORMAT),
                                TextureAccess::ReadWrite,
                                vk::Filter::eLinear,
                                vk::SamplerAddressMode::eRepeat,
                                glm::vec4(0),
                                PIGMENT_LAYERS)
                .AllocPointer);
    }

    void WaterColorSimBuffers::ReallocateRender(const RenderContext& context)
    {
        PigmentRender = static_cast<RenderTexture*>(
            context.BlackBox->ReallocateIfNeeded(PIGMENT_RENDER,
                                SIMULATION_RES_X,
                                SIMULATION_RES_Y,
                                static_cast<VkFormat>(PIGMENT_RENDER_FORMAT),
                                TextureAccess::ReadWrite,
                                vk::Filter::eLinear,
                                vk::SamplerAddressMode::eRepeat,
                                CANVAS_COLOR)
                .AllocPointer);
    }
} // namespace Beer::Rendering
