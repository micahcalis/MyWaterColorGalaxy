#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "vulkan/vulkan.hpp"

namespace Beer::Core
{
    class ShaderManager : public IAssetManager<Rendering::Shader>
    {
    private:
        const Device* device;
        const Swapchain* swapchain;
        vk::DescriptorSetLayout globalSetLayout = nullptr;
        vk::Format depthFormat = vk::Format::eUndefined;

    public:
        ShaderManager(const Device* device,
            const Swapchain* swapchain,
            vk::DescriptorSetLayout globalSetLayout,
            vk::Format depthFormat)
            : device(device), swapchain(swapchain), globalSetLayout(globalSetLayout), depthFormat(depthFormat)
        {
            Initialize();
        }

        ~ShaderManager()
        {
            Clear();
        }

        const vk::DescriptorSetLayout& GetGlobalSetLayout() const { return globalSetLayout; }
        const vk::Format GetDepthFormat() const { return depthFormat; }

        std::shared_ptr<Rendering::Shader> Load(const std::filesystem::path& path) override;

    private:
        void Initialize();
        void UpdateGlobals();
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
