#include "Core/Application/VulkanInitUtilities.hpp"
#include <iostream>

namespace Beer::Core
{
    std::vector<char const*> VulkanInitUtilities::GetRequiredLayers(const bool enableValidationLayers,
        const std::vector<char const*>& validationLayers,
        const vk::raii::Context& context)
    {
        std::vector<char const*> requiredLayers;

        if (enableValidationLayers)
        {
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());
        }

        auto layerProperties = context.enumerateInstanceLayerProperties();

        if (ValidationLayersUnsupported(requiredLayers, layerProperties))
        {
            throw std::runtime_error("One or more required layers are not supported!");
        }

        return requiredLayers;
    }

    bool VulkanInitUtilities::ValidationLayersUnsupported(std::vector<char const*> requiredLayers, std::vector<vk::LayerProperties> layerProperties)
    {
        return std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
            return std::ranges::none_of(layerProperties,
                [requiredLayer](auto const& layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
        });
    }

    VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanInitUtilities::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
        vk::DebugUtilsMessageTypeFlagsEXT type,
        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void*)
    {
        std::cerr << "validation layer: type" << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
        return vk::False;
    }

    void VulkanInitUtilities::FilterPhysicalDevices(std::vector<vk::raii::PhysicalDevice>& devices,
        const std::vector<const char*>& deviceExtensions)
    {
        auto devIter = std::erase_if(devices, [&](const vk::raii::PhysicalDevice& device) {
            auto queueFamilies = device.getQueueFamilyProperties();
            bool hasGraphicsQueue = std::ranges::any_of(queueFamilies,
                [](const vk::QueueFamilyProperties& qfp) {
                    return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics;
                });

            bool correctVersion = device.getProperties().apiVersion >= VK_API_VERSION_1_3;

            auto availableExtensions = device.enumerateDeviceExtensionProperties();

            bool extensionsSupported = std::ranges::all_of(deviceExtensions,
                [&](const char* requiredExt) {
                    return std::ranges::any_of(availableExtensions,
                        [requiredExt](const vk::ExtensionProperties& availExt) {
                            return std::strcmp(availExt.extensionName, requiredExt) == 0;
                        });
                });

            bool isSuitable = hasGraphicsQueue && correctVersion && extensionsSupported;
            return !isSuitable;
        });
    }

    uint32_t VulkanInitUtilities::GetPhysicalDeviceScore(const vk::raii::PhysicalDevice& device, bool& hasGeomShader)
    {
        uint32_t score = 0;
        auto properties = device.getProperties();
        auto features = device.getFeatures();

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            score += 1000;
        }

        score += properties.limits.maxImageDimension2D;
        hasGeomShader = features.geometryShader;

        return score;
    }

    void VulkanInitUtilities::GetQueueFamilyIndices(const std::vector<vk::QueueFamilyProperties> queueFamilyProperties,
        uint32_t& graphicsIndex,
        uint32_t& presentIndex,
        const vk::raii::PhysicalDevice& physicalDevice,
        const vk::raii::SurfaceKHR& surface)
    {
        auto graphicsQueueFamilyProperty = std::find_if(queueFamilyProperties.begin(),
            queueFamilyProperties.end(),
            [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });

        graphicsIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));

        bool graphicsSupportSurface = physicalDevice.getSurfaceSupportKHR(graphicsIndex, *surface);
        presentIndex = graphicsSupportSurface ? graphicsIndex : static_cast<uint32_t>(queueFamilyProperties.size());

        if (!IndexIsCompatible(presentIndex, queueFamilyProperties.size()))
        {
            for (size_t i = 0; i < queueFamilyProperties.size(); i++)
            {
                bool supportsGraphics = static_cast<bool>(queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics);
                bool supportsPresent = physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface);

                if (supportsGraphics && supportsPresent)
                {
                    graphicsIndex = static_cast<uint32_t>(i);
                    presentIndex = graphicsIndex;
                    break;
                }
            }

            if (!IndexIsCompatible(presentIndex, queueFamilyProperties.size()))
            {
                for (size_t i = 0; i < queueFamilyProperties.size(); i++)
                {
                    bool supportsPresent = physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface);

                    if (supportsPresent)
                    {
                        presentIndex = static_cast<uint32_t>(i);
                        break;
                    }
                }
            }
        }
    }

    bool VulkanInitUtilities::IndexIsCompatible(const int presentIndex, const int queueFamilyLength)
    {
        return presentIndex != queueFamilyLength;
    }
} // namespace Beer::Core
