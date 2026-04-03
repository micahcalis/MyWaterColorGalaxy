#pragma once

#include "vulkan/vulkan_raii.hpp"
#include <cstdint>

namespace Beer::Rendering
{
    struct KernelSettings
    {
        std::string Name;
        uint32_t Index;
    };

    struct ComputeKernel
    {
    public:
        vk::raii::Pipeline Pipeline;
        KernelSettings Settings;

    public:
        ComputeKernel(vk::raii::Pipeline pipeline, const KernelSettings settings)
            : Pipeline(std::move(pipeline)), Settings(settings)
        {
        }
    };
} // namespace Beer::Rendering
