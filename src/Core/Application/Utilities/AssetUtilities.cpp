#include "Core/Application/Utilities/AssetUtilities.hpp"
#include <fstream>
#include <SDL3/SDL.h>

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

    std::filesystem::path AssetUtilities::GetBasePath(const std::string& subPath)
    {
        const char* basePath = SDL_GetBasePath();

        if (basePath != nullptr)
        {
            return std::filesystem::path(basePath) / subPath;
        }

        return std::filesystem::current_path() / subPath;
    }

    std::vector<uint32_t> AssetUtilities::LoadSpvFile(const std::filesystem::path& filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open SPIR-V file: " + filepath.string());
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        return buffer;
    }

    static constexpr std::string_view SHADER_HEAD = "assets/shaders/";
    static constexpr std::string_view SHADER_TAIL = ".spv";

    std::filesystem::path AssetUtilities::GetShaderPath(const std::string& shaderName)
    {
        std::string subPath = std::string(SHADER_HEAD) + shaderName + std::string(SHADER_TAIL);
        return GetBasePath(subPath);
    }

    static constexpr std::string_view JSON_TAIL = ".json";

    std::filesystem::path AssetUtilities::GetShaderJsonPath(const std::string& shaderName)
    {
        std::string subPath = std::string(SHADER_HEAD) + shaderName + std::string(JSON_TAIL);
        return GetBasePath(subPath);
    }

    static constexpr std::string_view MODEL_HEAD = "assets/models/";
    static constexpr std::string_view MODELOBJ_TAIL = ".obj";
    static constexpr std::string_view MODELFBX_TAIL = ".fbx";

    std::filesystem::path AssetUtilities::GetModelPath(const std::string& modelName, bool isObj)
    {
        std::string subPath = std::string(MODEL_HEAD) + modelName + std::string(isObj ? MODELOBJ_TAIL : MODELFBX_TAIL);
        return GetBasePath(subPath);
    }

    static constexpr std::string_view TEXTURE_HEAD = "assets/textures/";
    static constexpr std::string_view TEXTURE_TAIL = ".png";

    std::filesystem::path AssetUtilities::GetTexturePath(const std::string& textureName)
    {
        std::string subPath = std::string(TEXTURE_HEAD) + textureName + std::string(TEXTURE_TAIL);
        return GetBasePath(subPath);
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
