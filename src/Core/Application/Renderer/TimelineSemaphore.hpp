#pragma once

#include "Core/Application/Renderer/Device.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <cstdint>

namespace Beer::Core
{
    class TimelineSemaphore
    {
    private:
        vk::raii::Semaphore semaphoreHandle = nullptr;
        uint64_t counter = 0;
        uint64_t currentSubmitTicket = 0;

    public:
        TimelineSemaphore(const Device& device);
        uint64_t AssignTicket();
        vk::TimelineSemaphoreSubmitInfo GetSubmitInfo();
        uint64_t GetCurrentSignValue(const Device& device) const;
        vk::Semaphore GetHandle() const { return *semaphoreHandle; }
    };
} // namespace Beer::Core
