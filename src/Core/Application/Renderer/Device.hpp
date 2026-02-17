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
        [[nodiscard]] const vk::raii::PhysicalDevice& GetPhysicalDevice() const;
        [[nodiscard]] const vk::raii::Device& GetLogicalDevice() const;
        [[nodiscard]] const vk::raii::Queue& GetGraphicsQueue() const;
        [[nodiscard]] const vk::raii::Queue& GetPresentQueue() const;
        [[nodiscard]] uint32_t GetGraphicsIndex() const;
        [[nodiscard]] vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities(const vk::raii::SurfaceKHR& surface) const;
        [[nodiscard]] std::vector<vk::SurfaceFormatKHR> GetAvailableFormats(const vk::raii::SurfaceKHR& surface) const;
        [[nodiscard]] std::vector<vk::PresentModeKHR> GetAvailablePresentModes(const vk::raii::SurfaceKHR& surface) const;

    private:
        void PickPhysicalDevice(const vk::raii::Instance& instance);
        void CreateLogicalDevice(const vk::raii::SurfaceKHR& surface);
    };
} // namespace Beer::Core
