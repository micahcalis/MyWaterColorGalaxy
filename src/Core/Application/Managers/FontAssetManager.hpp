#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "Rendering/Text/FontAsset.hpp"

namespace Beer::Core
{
    class FontAssetManager : public IAssetManager<Rendering::FontAsset>
    {
    private:
        const Device* device;
        UploadManager* uploadManager;

        std::shared_ptr<Rendering::Shader> textShader;

    public:
        FontAssetManager(const Device* device, UploadManager* uploadManager)
            : device(device), uploadManager(uploadManager)
        {
            textShader = Rendering::Shader::Get("UI/TextMeshDefault");
        }

        ~FontAssetManager()
        {
            Clear();
        }

        std::shared_ptr<Rendering::FontAsset> Load(const std::filesystem::path& path) override;
        Rendering::Shader* GetTextShader() { return textShader.get(); }

    private:
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
