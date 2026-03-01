#pragma once

#include "Core/Application/Managers/IAssetManager.hpp"
#include "Core/Application/Managers/UploadManager.hpp"
#include "Rendering/Buffer/BufferAllocator.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include <memory>

namespace Beer::Core
{
    class MeshManager : public IAssetManager<Rendering::Mesh>
    {
    private:
        const std::shared_ptr<Rendering::BufferAllocator> bufferAllocator;
        UploadManager* uploadManager;

    public:
        MeshManager(const std::shared_ptr<Rendering::BufferAllocator> bufferAllocator,
            UploadManager* uploadManager)
            : bufferAllocator(bufferAllocator), uploadManager(uploadManager)
        {
            Initialize();
        }

        ~MeshManager()
        {
            Clear();
        }

    public:
        std::shared_ptr<Rendering::Mesh> Load(const std::filesystem::path& path) override;

    private:
        void Initialize();
        const std::filesystem::path GetPath(const std::string& name) override;
    };
} // namespace Beer::Core
