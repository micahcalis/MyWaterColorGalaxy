#pragma once

#include <string>
#include "Rendering/Pipeline/Frame/Dependency/PassDependency.hpp"
#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Rendering
{
    const std::string MAIN_COLOR = "MainColor";
    const std::string MAIN_DEPTH = "MainDepth";
    const std::string GBUFFER_NORMAL = "GBufferNormal";
    const std::string GBUFFER_ALBEDO = "GBufferAlbedo";
    const std::string GBUFFER_MAT = "GBufferMaterial";
    const VkFormat GBUFFER_NORMAL_FORMAT = VK_FORMAT_R16G16_SFLOAT;
    const VkFormat GBUFFER_ALBEDO_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;
    const VkFormat GBUFFER_MAT_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;

    const std::string OPAQUE_PASS = "Opaque";
    const std::string SKYBOX_PASS = "Skybox";
    const std::string DEFERRED_SHADE_PASS = "DeferredShade";
    const std::string TRANSPARENT_PASS = "Transparent";
    const std::string UI_PASS = "UserInterface";
    const std::string INTERACTIVE_PAINT_PASS = "InteractivePaint";

    constexpr static uint32_t TRANSFORM_BUFFER_COUNT = 100'000;
    constexpr std::string_view TRANSFORM_BUFFER_NAME = "TransformInstancingBuffer";

    class RenderGlobalUtilities
    {
    public:
        static std::vector<PassDependency> GetGBufferDependencies(bool write)
        {
            std::vector<PassDependency> dependencies;
            dependencies.reserve(3);

            ResourceAction resourceAction = write ? ResourceAction::ColorWrite : ResourceAction::ColorRead;

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_ALBEDO),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                vk::Format::eB8G8R8A8Srgb));

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_NORMAL),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                static_cast<vk::Format>(VK_FORMAT_R16G16_SFLOAT)));

            dependencies.emplace_back(PassDependency(std::string(GBUFFER_MAT),
                resourceAction,
                ResetOperator::ClearColor({0, 0, 0, 0}),
                vk::Format::eB8G8R8A8Srgb));

            return dependencies;
        }
    };
} // namespace Beer::Rendering
