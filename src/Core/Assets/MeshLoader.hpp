#pragma once

#include "Core/Assets/MeshAsset.hpp"
#include <filesystem>

namespace Beer::Core
{
    class MeshLoader
    {
    public:
        static MeshAsset LoadMesh(const std::filesystem::path& path);

    private:
        static MeshAsset LoadObj(const std::filesystem::path& path);
        static MeshAsset LoadFBX(const std::filesystem::path& path);
        static bool IsObj(const std::filesystem::path& path);
    };
} // namespace Beer::Core
