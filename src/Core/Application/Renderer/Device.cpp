#include "Core/Application/Renderer/Device.hpp"
#include "Core/Application/Utilities/VulkanInitUtilities.hpp"
#include "vulkan/vulkan.hpp"
#include <map>
#include <iostream>
#include <set>

namespace Beer::Core
{
    void Device::Initialize(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface)
    {
        PickPhysicalDevice(instance);
        CreateLogicalDevice(surface);
    }

    const vk::raii::PhysicalDevice& Device::GetPhysicalDevice() const { return physicalDevice; }
    const vk::raii::Device& Device::GetLogicalDevice() const { return device; }
    const vk::raii::Queue& Device::GetGraphicsQueue() const { return graphicsQueue; }
    const vk::raii::Queue& Device::GetPresentQueue() const { return presentQueue; }
    const uint32_t Device::GetGraphicsIndex() const { return graphicsIndex; }

    vk::SurfaceCapabilitiesKHR Device::GetSurfaceCapabilities(const vk::raii::SurfaceKHR& surface) const
    {
        return physicalDevice.getSurfaceCapabilitiesKHR(*surface);
    }

    std::vector<vk::SurfaceFormatKHR> Device::GetAvailableFormats(const vk::raii::SurfaceKHR& surface) const
    {
        return physicalDevice.getSurfaceFormatsKHR(surface);
    }

    const std::vector<vk::PresentModeKHR> Device::GetAvailablePresentModes(const vk::raii::SurfaceKHR& surface) const
    {
        return physicalDevice.getSurfacePresentModesKHR(*surface);
    }

    void Device::PickPhysicalDevice(const vk::raii::Instance& instance)
    {
        auto devices = instance.enumeratePhysicalDevices();

        if (devices.empty())
        {
            throw std::runtime_error("failed to find GPU's with Vulkan support");
        }

        VulkanInitUtilities::FilterPhysicalDevices(devices,
            deviceExtensions);

        std::multimap<int, vk::raii::PhysicalDevice> deviceCandidates;

        for (const auto& device : devices)
        {
            bool hasGeomShader = false;
            uint32_t score = VulkanInitUtilities::GetPhysicalDeviceScore(device, hasGeomShader);

            if (!hasGeomShader)
            {
                continue;
            }

            deviceCandidates.insert(std::make_pair(score, device));
        }

        if (deviceCandidates.rbegin()->first > 0)
        {
            physicalDevice = deviceCandidates.rbegin()->second;
            std::cout << "Physical Device Found: " << physicalDevice.getProperties().deviceName << std::endl;
        } else
        {
            throw std::runtime_error("failed to find a suitable GPU");
        }
    }

    void Device::CreateLogicalDevice(const vk::raii::SurfaceKHR& surface)
    {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        graphicsIndex = 0;
        uint32_t presentIndex = 0;

        VulkanInitUtilities::GetQueueFamilyIndices(queueFamilyProperties,
            graphicsIndex,
            presentIndex,
            physicalDevice,
            surface);

        bool graphicsCompatible = VulkanInitUtilities::IndexIsCompatible(graphicsIndex,
            queueFamilyProperties.size());

        bool presentCompatible = VulkanInitUtilities::IndexIsCompatible(presentIndex,
            queueFamilyProperties.size());

        if (!graphicsCompatible || !presentCompatible)
        {
            throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {graphicsIndex, presentIndex};

        float queuePriority = 0.5f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            vk::DeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain{};

        featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
        featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
        featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = true;
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

        vk::DeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        device = vk::raii::Device(physicalDevice, deviceCreateInfo);
        graphicsQueue = vk::raii::Queue(device, graphicsIndex, 0);
        presentQueue = vk::raii::Queue(device, presentIndex, 0);
    }
} // namespace Beer::Core
