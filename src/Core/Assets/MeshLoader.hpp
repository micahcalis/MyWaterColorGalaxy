#pragma once

#include "Core/Assets/MeshAsset.hpp"
#include <string>
#include <filesystem>

namespace Beer::Core
{
    class MeshLoader
    {
    public:
        static MeshAsset LoadMesh(const std::string& name, bool isObj = true);

    private:
        static MeshAsset LoadObj(const std::filesystem::path& path);
        static MeshAsset LoadFBX(const std::filesystem::path& path);
    };
} // namespace Beer::Core
