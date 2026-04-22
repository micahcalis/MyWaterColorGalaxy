#pragma once

#include <string>
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    static const uint32_t SIMULATION_RES_X = 512;
    static const uint32_t SIMULATION_RES_Y = 512;

    static const std::string CANVAS_BUFFER = "CanvasBuffer";
    static const vk::Format CANVAS_BUFFER_FORMAT = vk::Format::eR16G16B16A16Unorm;
    static const std::string SUSPENDED_PIGMENT = "SuspendedPigment";
    static const vk::Format SUSPENDED_PIGMENT_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string DEPOSITED_PIGMENT = "DepositedPigment";
    static const vk::Format DEPOSITED_PIGMENT_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string SHALLOW_WATER = "ShallowWater";
    static const vk::Format SHALLOW_WATER_FORMAT = vk::Format::eR32G32B32A32Sfloat;
    static const std::string PIGMENT_RENDER = "PigmentRender";
    static const vk::Format PIGMENT_RENDER_FORMAT = vk::Format::eR16G16B16A16Unorm;

    static const uint32_t CANVAS_GENERATION_KERNEL = 0;
    static const uint32_t PAINT_INJECT_KERNEL = 1;

    class WaterColorSimBuffers
    {
    public:
        RenderTexture* CanvasBuffer = nullptr;
        RenderTexture* SuspendedPigment = nullptr;
        RenderTexture* DepositedPigment = nullptr;
        RenderTexture* ShallowWater = nullptr;
        RenderTexture* PigmentRender = nullptr;
        std::shared_ptr<ComputeContext> SimulationContext;
        Material* debugMaterial = nullptr;

    public:
        WaterColorSimBuffers(Material* debugMaterial);
        void ReallocateCanvas(const RenderContext& context);
        void ReallocateWater(const RenderContext& context);
    };
} // namespace Beer::Rendering
