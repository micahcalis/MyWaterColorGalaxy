#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Core/Application/Renderer/Device.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Buffer/Image.hpp"
#include <memory>

namespace Beer::Core
{
    class ImageAssetManager : public IAssetManager<Rendering::Image>
    {
        const Device* device;
        UploadManager* uploadManager;

    public:
        ImageAssetManager(const Device* device,
            const std::shared_ptr<Rendering::BufferAllocator> bufferAllocator,
            UploadManager* uploadManager)
            : device(device), uploadManager(uploadManager)
        {
            InitializeTextureFallback();
        }

        ~ImageAssetManager()
        {
            Clear();
        }

        std::shared_ptr<Rendering::Image> Load(const std::filesystem::path& path) override;

    private:
        void InitializeTextureFallback();
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
