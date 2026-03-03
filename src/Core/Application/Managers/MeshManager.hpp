#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include <memory>

namespace Beer::Core
{
    class MeshManager : public IAssetManager<Rendering::Mesh>
    {
    private:
        UploadManager* uploadManager;

    public:
        MeshManager(UploadManager* uploadManager)
            : uploadManager(uploadManager)
        {
        }

        ~MeshManager()
        {
            Clear();
        }

    public:
        std::shared_ptr<Rendering::Mesh> Load(const std::filesystem::path& path) override;

    private:
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
