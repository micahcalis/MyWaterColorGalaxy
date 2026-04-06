#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Rendering/Text/FontAsset.hpp"

namespace Beer::Core
{
    class FontAssetManager : public IAssetManager<Rendering::FontAsset>
    {
    private:
        const Device* device;
        UploadManager* uploadManager;

    public:
        FontAssetManager(const Device* device, UploadManager* uploadManager)
            : device(device), uploadManager(uploadManager)
        {
        }

        ~FontAssetManager()
        {
            Clear();
        }

        std::shared_ptr<Rendering::FontAsset> Load(const std::filesystem::path& path) override;

    private:
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
