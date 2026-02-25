#pragma once

#include "Core/Application/Jobs/IUploadJob.hpp"
#include <memory>
#include "Core/Assets/MeshAsset.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Mesh/Mesh.hpp"

namespace Beer::Core
{
    class MeshUploadJob : public IUploadJob
    {
    private:
        std::shared_ptr<Rendering::Mesh> mesh;
        MeshAsset meshAsset;

    public:
        MeshUploadJob(std::shared_ptr<Rendering::Mesh> mesh,
            MeshAsset meshAsset)
            : mesh(mesh), meshAsset(std::move(meshAsset)), IUploadJob(nullptr, meshAsset.GetTotalSize())
        {
        }

        void Execute(std::shared_ptr<Rendering::Buffer> stagingBuffer,
            vk::raii::CommandBuffer& commandBuffer,
            const size_t offset) override;
    };
} // namespace Beer::Core
