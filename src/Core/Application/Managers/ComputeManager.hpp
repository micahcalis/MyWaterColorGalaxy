#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Rendering/Compute/ComputeShader.hpp"

namespace Beer::Core
{
    class ComputeManager : public IAssetManager<Rendering::ComputeShader>
    {
        const Device* device;
        const Swapchain* swapchain;

    public:
        ComputeManager(const Device* device,
            const Swapchain* swapchain)
            : device(device), swapchain(swapchain)
        {
        }

        ~ComputeManager()
        {
            Clear();
        }

        std::shared_ptr<Rendering::ComputeShader> Load(const std::filesystem::path& path) override;

    private:
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
