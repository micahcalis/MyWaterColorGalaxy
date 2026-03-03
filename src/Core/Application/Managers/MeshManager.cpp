#include "Core/Application/Managers/MeshManager.hpp"
#include "Core/Application/Utilities/AssetUtilities.hpp"
#include "Core/Assets/MeshAsset.hpp"
#include "Core/Assets/MeshLoader.hpp"
#include "Rendering/Mesh/MeshBuffers.hpp"
#include "Core/Application/Jobs/MeshUploadJob.hpp"
#include <memory>

namespace Beer::Core
{
    std::shared_ptr<Rendering::Mesh> MeshManager::Load(const std::filesystem::path& path)
    {
        MeshAsset meshAsset = MeshLoader::LoadMesh(path);
        Rendering::MeshBuffers meshBuffers = Rendering::MeshBuffers(meshAsset);

        std::shared_ptr<Rendering::Mesh> mesh = std::make_shared<Rendering::Mesh>(
            std::move(meshBuffers),
            meshAsset.GetVertexCount(),
            meshAsset.GetIndexCount());

        std::unique_ptr<MeshUploadJob> uploadJob = std::make_unique<MeshUploadJob>(
            mesh, meshAsset);

        uploadManager->AddJob(std::move(uploadJob));

        return mesh;
    }

    const std::filesystem::path MeshManager::GetPath(const std::string& name)
    {
        return AssetUtilities::GetModelPath(name);
    }
} // namespace Beer::Core
