#pragma once

#include "vulkan/vulkan.hpp"
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Core
{
    class Device
    {
    private:
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};
        vk::raii::Device device = nullptr;
        vk::PhysicalDeviceFeatures deviceFeatures;
        vk::raii::Queue graphicsQueue = nullptr;
        vk::raii::Queue presentQueue = nullptr;
        uint32_t graphicsIndex;

    public:
        void Initialize(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface);
        const vk::raii::PhysicalDevice& GetPhysicalDevice() const;
        const vk::raii::Device& GetLogicalDevice() const;
        const vk::raii::Queue& GetGraphicsQueue() const;
        const vk::raii::Queue& GetPresentQueue() const;
        const uint32_t GetGraphicsIndex() const;
        vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities(const vk::raii::SurfaceKHR& surface) const;
        std::vector<vk::SurfaceFormatKHR> GetAvailableFormats(const vk::raii::SurfaceKHR& surface) const;
        const std::vector<vk::PresentModeKHR> GetAvailablePresentModes(const vk::raii::SurfaceKHR& surface) const;

    private:
        void PickPhysicalDevice(const vk::raii::Instance& instance);
        void CreateLogicalDevice(const vk::raii::SurfaceKHR& surface);
    };
} // namespace Beer::Core
