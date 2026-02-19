#include "Core/Application/Utilities/AssetUtilities.hpp"
#include <fstream>
#include <SDL3/SDL.h>
#include <print>

namespace Beer::Core
{
    [[nodiscard]] std::vector<char> AssetUtilities::ReadFile(const std::filesystem::path& path)
    {
        std::ifstream file(path.string(), std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error(std::format("ERROR: Failed to open at path '{}'!", path.string()));
        }

        std::vector<char> buffer(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        file.close();

        return buffer;
    }

    std::string AssetUtilities::GetAssetPath(const std::string& subPath)
    {
        const char* basePath = SDL_GetBasePath();

        if (basePath != nullptr)
        {
            std::string fullPath = std::string(basePath) + subPath;
            std::println("{}", fullPath);
            return fullPath;
        }

        return subPath;
    }

    static constexpr std::string_view SHADER_HEAD = "assets/shaders/";
    static constexpr std::string_view SHADER_TAIL = ".spv";

    std::string AssetUtilities::GetShaderPath(const std::string& shaderName)
    {
        std::string subPath = std::string(SHADER_HEAD) + shaderName + std::string(SHADER_TAIL);
        return GetAssetPath(subPath);
    }

    static constexpr std::string_view MODEL_HEAD = "assets/models/";
    static constexpr std::string_view MODEL_TAIL = ".obj";

    std::string AssetUtilities::GetModelPath(const std::string& modelName)
    {
        std::string subPath = std::string(MODEL_HEAD) + modelName + std::string(MODEL_TAIL);
        return GetAssetPath(subPath);
    }

    static constexpr std::string_view TEXTURE_HEAD = "assets/textures/";
    static constexpr std::string_view TEXTURE_TAIL = ".png";

    std::string AssetUtilities::GetTexturePath(const std::string& textureName)
    {
        std::string subPath = std::string(TEXTURE_HEAD) + textureName + std::string(TEXTURE_TAIL);
        return GetAssetPath(subPath);
    }

    [[nodiscard]] vk::raii::ShaderModule AssetUtilities::CreateShaderModule(const std::vector<char>& code,
        const vk::raii::Device& device)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = code.size() * sizeof(char);
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        vk::raii::ShaderModule shaderModule = vk::raii::ShaderModule(device, createInfo);
        return shaderModule;
    }
} // namespace Beer::Core
