#include "Core/Application/Renderer/TimelineSemaphore.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    TimelineSemaphore::TimelineSemaphore(const Device& device)
    {
        vk::SemaphoreTypeCreateInfo timelineCreateInfo;
        timelineCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
        timelineCreateInfo.initialValue = 0;

        vk::SemaphoreCreateInfo createInfo;
        createInfo.pNext = &timelineCreateInfo;

        semaphoreHandle = vk::raii::Semaphore(device.GetLogicalDevice(), createInfo);
    }

    uint64_t TimelineSemaphore::AssignTicket()
    {
        counter++;
        currentSubmitTicket = counter;
        return currentSubmitTicket;
    }

    vk::TimelineSemaphoreSubmitInfo TimelineSemaphore::GetSubmitInfo()
    {
        vk::TimelineSemaphoreSubmitInfo submitInfo{};
        submitInfo.waitSemaphoreValueCount = 1;
        submitInfo.pWaitSemaphoreValues = nullptr;
        submitInfo.signalSemaphoreValueCount = 1;
        submitInfo.pSignalSemaphoreValues = &currentSubmitTicket;

        return submitInfo;
    }

    uint64_t TimelineSemaphore::GetCurrentSignValue(const Device& device) const
    {
        vk::Device logicalDevice = *device.GetLogicalDevice();
        return logicalDevice.getSemaphoreCounterValue(GetHandle());
    }
} // namespace Beer::Core
