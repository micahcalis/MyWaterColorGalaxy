#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Renderer/Swapchain.hpp"
#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Core
{
    class ShaderManager : public IAssetManager<Rendering::Shader>
    {
    private:
        const Device* device;
        const Swapchain* swapchain;
        vk::Format depthFormat = vk::Format::eUndefined;
        std::unique_ptr<Rendering::ShaderGlobalsHandler> globalsHandler;

    public:
        ShaderManager(const Device* device,
            const Swapchain* swapchain,
            vk::Format depthFormat,
            uint32_t framesInFlight)
            : device(device), swapchain(swapchain), depthFormat(depthFormat), globalsHandler(std::make_unique<Rendering::ShaderGlobalsHandler>(device))
        {
            Initialize();
        }

        ~ShaderManager()
        {
            Clear();
        }

        const vk::Format GetDepthFormat() const { return depthFormat; }
        Rendering::ShaderGlobalsHandler* GetGlobalsHandler() const { return globalsHandler.get(); }

        std::shared_ptr<Rendering::Shader> Load(const std::filesystem::path& path) override;

    private:
        void Initialize();
        void UpdateGlobals();
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
