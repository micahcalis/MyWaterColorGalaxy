#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include "Core/Assets/MeshAsset.hpp"
#include <memory>
#include <vector>
#include "Rendering/Mesh/MeshBuffers.hpp"

namespace Beer::Core
{
    class MeshUtilities
    {
    public:
        static std::vector<std::unique_ptr<IUploadJob>> GetMeshAssetUploads(MeshAsset& meshAsset,
            Rendering::MeshBuffers& buffers,
            const std::shared_ptr<Rendering::BufferAllocator>& bufferAllocator);

    private:
        static void TryEmplaceData(std::vector<std::unique_ptr<IUploadJob>>& uploadJobs,
            void* data,
            vk::DeviceSize bufferSize,
            std::shared_ptr<Rendering::Buffer>& buffer,
            const std::shared_ptr<Rendering::BufferAllocator>& bufferAllocator,
            bool isVertAttrib);
    };
} // namespace Beer::Core
