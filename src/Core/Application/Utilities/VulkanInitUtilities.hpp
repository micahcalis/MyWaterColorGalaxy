#pragma once

#include <vector>
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Core
{
    class VulkanInitUtilities
    {
    public:
        static std::vector<char const*> GetRequiredLayers(const bool enableValidationLayers,
            const std::vector<char const*>& validationLayers,
            const vk::raii::Context& context);

        static bool ValidationLayersUnsupported(std::vector<char const*> requiredLayers,
            std::vector<vk::LayerProperties> layerProperties);

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
            vk::DebugUtilsMessageTypeFlagsEXT type,
            const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void*);

        static void FilterPhysicalDevices(std::vector<vk::raii::PhysicalDevice>& devices,
            const std::vector<const char*>& deviceExtensions);

        static uint32_t GetPhysicalDeviceScore(const vk::raii::PhysicalDevice& device, bool& hasGeomShader);

        static void GetQueueFamilyIndices(const std::vector<vk::QueueFamilyProperties> queueFamilyProperties,
            uint32_t& graphicsComputeIndex,
            uint32_t& presentIndex,
            const vk::raii::PhysicalDevice& physicalDevice,
            const vk::raii::SurfaceKHR& surface);

        static bool IndexIsCompatible(const int presentIndex, const int queueFamilyLength);
    };
} // namespace Beer::Core
