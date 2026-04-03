#pragma once

#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Buffer/SSBOType.hpp"
#include "Rendering/Pipeline/Frame/Resource/IRenderResource.hpp"
#include "Rendering/Pipeline/Frame/ReallocData.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include "Rendering/Texture/RenderTexture.hpp"
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"
#include <type_traits>

namespace Beer::Rendering
{
    class FrameBlackbox
    {
    private:
        std::unordered_map<std::string, std::unique_ptr<IRenderResource>> blackbox;
        const Core::Device* device;
        Core::UploadManager* uploadManager;

    public:
        FrameBlackbox(const Core::Device* device, Core::UploadManager* uploadManager)
            : device(device), uploadManager(uploadManager)
        {
        }

        RenderTexture* CreateRenderTexture2D(const std::string& name,
            uint32_t width,
            uint32_t height,
            VkFormat format,
            TextureAccess access = TextureAccess::Standard,
            vk::Filter filter = vk::Filter::eLinear,
            vk::SamplerAddressMode tiling = vk::SamplerAddressMode::eRepeat,
            glm::vec4 clearColor = glm::vec4(0));

        PhaseBuffer* CreatePhaseBuffer(const std::string& name,
            VkDeviceSize size,
            SSBOType type = SSBOType::Hybrid);

        template<typename T>
        T* GetResource(const std::string& name)
        {
            static_assert(std::is_base_of_v<IRenderResource, T>, "Requested type must inherit from IRenderResource!");

            auto it = blackbox.find(name);
            if (it != blackbox.end())
            {
                return static_cast<T*>(it->second.get());
            } else
            {
                return nullptr;
            }
        }

        template<typename T>
        void ReleaseResource(const std::string& name)
        {
            static_assert(std::is_base_of_v<IRenderResource, T>, "Requested type must inherit from IRenderResource!");

            auto it = blackbox.find(name);
            if (it != blackbox.end())
            {
                blackbox.erase(it);
            }
        }

        ReallocRT ReallocateIfNeeded(const std::string& name,
            uint32_t width,
            uint32_t height,
            VkFormat format,
            TextureAccess access = TextureAccess::Standard,
            vk::Filter filter = vk::Filter::eLinear,
            vk::SamplerAddressMode tiling = vk::SamplerAddressMode::eRepeat,
            glm::vec4 clearColor = glm::vec4(0));

        ReallocPB ReallocateIfNeeded(const std::string& name,
            VkDeviceSize size,
            SSBOType type = SSBOType::Hybrid);

    private:
        std::shared_ptr<Image> CreateRenderTextureImage(uint32_t width,
            uint32_t height,
            VkFormat format,
            TextureAccess access = TextureAccess::Standard,
            glm::vec4 clearColor = glm::vec4(0));

        std::shared_ptr<Buffer> CreateSSBOHandle(VkDeviceSize size, SSBOType type);
    };
} // namespace Beer::Rendering
