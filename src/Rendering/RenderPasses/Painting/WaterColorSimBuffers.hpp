#pragma once

#include <string>
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    static const uint32_t SIMULATION_RES_X = 512;
    static const uint32_t SIMULATION_RES_Y = 512;
    static const uint32_t PIGMENT_LAYERS = 3;

    static const std::string CANVAS_BUFFER = "CanvasBuffer";
    static const vk::Format CANVAS_BUFFER_FORMAT = vk::Format::eR16G16B16A16Unorm;
    static const std::string SUSPENDED_PIGMENT_A = "SuspendedPigmentPongA";
    static const std::string SUSPENDED_PIGMENT_B = "SuspendedPigmentPongB";
    static const vk::Format SUSPENDED_PIGMENT_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string DEPOSITED_PIGMENT = "DepositedPigment";
    static const vk::Format DEPOSITED_PIGMENT_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string SHALLOW_WATER = "ShallowWater";
    static const vk::Format SHALLOW_WATER_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string FLUX_BUFFER = "FluxBuffer";
    static const vk::Format FLUX_BUFFER_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string PIGMENT_RENDER = "PigmentRender";
    static const vk::Format PIGMENT_RENDER_FORMAT = vk::Format::eR16G16B16A16Unorm;
    static const glm::vec4 CANVAS_COLOR = glm::vec4(0.969f, 0.969f, 0.914, 1);

    static const uint32_t CANVAS_GENERATION_KERNEL = 0;
    static const uint32_t PAINT_INJECT_KERNEL = 1;
    static const uint32_t CALC_FLUX_KERNEL = 2;
    static const uint32_t PIGMENT_FLUX_KERNEL = 3;
    static const uint32_t FLUID_FLUX_KERNEL = 4;
    static const uint32_t TRANSFER_PGMNT_KERNEL = 5;
    static const uint32_t RENDER_PGMNT_KERNEL = 6;

    class WaterColorSimBuffers
    {
    public:
        RenderTexture* CanvasBuffer = nullptr;
        RenderTexture* SuspendedPigmentPongA = nullptr;
        RenderTexture* SuspendedPigmentPongB = nullptr;
        RenderTexture* DepositedPigment = nullptr;
        RenderTexture* ShallowWater = nullptr;
        RenderTexture* FluxBuffer = nullptr;
        RenderTexture* PigmentRender = nullptr;
        std::shared_ptr<ComputeContext> SimulationContext;
        Material* debugMaterial = nullptr;

    public:
        WaterColorSimBuffers(Material* debugMaterial);
        void ReallocateCanvas(const RenderContext& context);
        void ReallocateWater(const RenderContext& context);
        void ReallocateFlux(const RenderContext& context);
        void ReallocateSuspended(const RenderContext& context);
        void ReallocateDeposited(const RenderContext& context);
        void ReallocateRender(const RenderContext& context);

        RenderTexture* GetSuspendedPong(bool isSource)
        {
            bool frame = UniformDescriptor::GetFrameIndex() % 2 == 0;
            return frame == isSource ? SuspendedPigmentPongA : SuspendedPigmentPongB;
        }
    };
} // namespace Beer::Rendering
