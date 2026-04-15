#pragma once

#include <vector>
#include <filesystem>
#include <vulkan/vulkan_raii.hpp>
#include <filesystem>

namespace Beer::Core
{
    class AssetUtilities
    {
    public:
        [[nodiscard]] static std::vector<char> ReadFile(const std::filesystem::path& path);
        static std::vector<uint32_t> LoadSpvFile(const std::filesystem::path& filepath);
        static std::filesystem::path GetBasePath(const std::string& subPath);
        static std::filesystem::path GetShaderPath(const std::string& shaderName);
        static std::filesystem::path GetComputeShaderPath(const std::string& computeName);
        static std::filesystem::path GetShaderJsonPath(const std::string& shaderName);
        static std::filesystem::path GetShaderJsonPath(const std::filesystem::path& spvPath);
        static std::filesystem::path GetModelPath(const std::string& modelName);
        static std::filesystem::path GetTexturePath(const std::string& textureName);
        static std::filesystem::path GetFontAssetPath(const std::string& fontName);
        static std::filesystem::path GetFontAssetJsonPath(const std::filesystem::path& pngPath);
        [[nodiscard]] static vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code,
            const vk::raii::Device& device);
    };
} // namespace Beer::Core
