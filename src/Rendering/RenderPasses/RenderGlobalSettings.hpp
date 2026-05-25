#pragma once

#include <string>
#include <string_view>
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResetOperator.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    static const std::string_view VIRTUAL_MAIN_COLOR = "VirtualMainColor";
    static const std::string MAIN_COLOR_A = "MainColorA";
    static const std::string MAIN_COLOR_B = "MainColorB";
    static const std::string MAIN_DEPTH = "MainDepth";
    static const std::string GBUFFER_NORMAL_OFFSET = "GBufferNormalOffset";
    static const std::string GBUFFER_ALBEDO = "GBufferAlbedo";
    static const std::string GBUFFER_MAT = "GBufferMaterial";
    static const std::string GBUFFER_EMISSION = "GBufferEmission";
    static const std::string GBUFFER_WATERCOLOR = "GBufferWatercolor";
    static const VkFormat GBUFFER_NORMAL_OFFSET_FORMAT = VK_FORMAT_R16G16B16A16_SFLOAT;
    static const VkFormat GBUFFER_ALBEDO_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;
    static const VkFormat GBUFFER_MAT_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;
    static const VkFormat GBUFFER_EMISSION_FORMAT = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
    static const VkFormat GBUFFER_WATERCOLOR_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;

    static const std::string OPAQUE_PASS = "Opaque";
    static const std::string SKYBOX_PASS = "Skybox";
    static const std::string DEFERRED_SHADE_PASS = "DeferredShade";
    static const std::string TRANSPARENT_PASS = "Transparent";
    static const std::string UI_PASS = "UserInterface";
    static const std::string INTERACTIVE_PAINT_PASS = "InteractivePaint";
    static const std::string WC_PROCESSING_PASS = "WatercolorProcessing";
    static const std::string WC_BLIT_PASS = "WatercolorBlit";
    static const std::string WC_BLUR_PASS_A = "WatercolorBlurA";
    static const std::string WC_BLUR_PASS_B = "WatercolorBlurB";

    constexpr static uint32_t TRANSFORM_BUFFER_COUNT = 100'000;
    constexpr std::string_view TRANSFORM_BUFFER_NAME = "TransformInstancingBuffer";

    class RenderGlobalUtilities
    {
    public:
        static std::vector<PassDependency> GetGBufferDependencies(bool write)
        {
            std::vector<PassDependency> dependencies;
            dependencies.reserve(4);

            ResourceAction resourceAction = write ? ResourceAction::ColorWrite : ResourceAction::ColorRead;

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_ALBEDO),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                static_cast<vk::Format>(GBUFFER_ALBEDO_FORMAT)));

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_NORMAL_OFFSET),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0.5f, 0.5f}),
                static_cast<vk::Format>(GBUFFER_NORMAL_OFFSET_FORMAT)));

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_MAT),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                static_cast<vk::Format>(GBUFFER_MAT_FORMAT)));

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_EMISSION),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                static_cast<vk::Format>(GBUFFER_EMISSION_FORMAT)));

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_WATERCOLOR),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                static_cast<vk::Format>(GBUFFER_WATERCOLOR_FORMAT)));

            return dependencies;
        }

        static std::vector<PassDependency> GetMainColorBlitDependencies()
        {
            std::vector<PassDependency> dependencies;
            dependencies.reserve(2);

            dependencies.emplace_back(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
                ResourceAction::ColorRead,
                ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
                static_cast<vk::Format>(Core::Screen::ColorFormat())));

            dependencies.emplace_back(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
                ResourceAction::ColorWrite,
                ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
                static_cast<vk::Format>(Core::Screen::ColorFormat())));

            return dependencies;
        }
    };
} // namespace Beer::Rendering
