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
        static std::string GetShaderPath(const std::string& shaderName);
        static std::string GetModelPath(const std::string& modelName);
        static std::string GetTexturePath(const std::string& textureName);
        [[nodiscard]] static vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code,
            const vk::raii::Device& device);
    };
} // namespace Beer::Core
