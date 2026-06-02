#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/Image.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/Compute/Threads.hpp"
#include <memory>

namespace Beer::Core
{
    class ImageAssetManager : public IAssetManager<Rendering::Image>
    {
    private:
        const Device* device;
        UploadManager* uploadManager;

    public:
        ImageAssetManager(const Device* device,
            const std::shared_ptr<Rendering::BufferAllocator> bufferAllocator,
            UploadManager* uploadManager)
            : device(device), uploadManager(uploadManager)
        {
            InitializeTextureFallbacks();
        }

        ~ImageAssetManager()
        {
            Clear();
        }

        std::shared_ptr<Rendering::Image> Load(const std::filesystem::path& path) override;

        std::shared_ptr<Rendering::Image> CreateEmpty2D(uint32_t width,
            uint32_t height,
            VkFormat format,
            uint32_t layerCount,
            bool isCubemap);

        std::shared_ptr<Rendering::Image> CreateEmpty3D(uint32_t width,
            uint32_t height,
            uint32_t depth,
            VkFormat format);

        void GenerateFromEmpty(std::shared_ptr<Rendering::Image> image,
            Rendering::ComputeContext* context,
            Rendering::Threads threads,
            uint32_t kernelIndex);

        void CopyImage(std::shared_ptr<Rendering::Image> sourceImage,
            std::shared_ptr<Rendering::Image> destinationImage);

    private:
        void InitializeTextureFallbacks();
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
