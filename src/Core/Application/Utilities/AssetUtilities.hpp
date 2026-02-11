#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan_raii.hpp>

namespace Beer::Core
{
    class AssetUtilities
    {
    public:
        [[nodiscard]] static std::vector<char> ReadFile(const std::filesystem::path& path);
        static std::string GetAssetPath(const std::string& subPath);
        [[nodiscard]] static vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code,
            const vk::raii::Device& device);
    };
} // namespace Beer::Core
