#include "Rendering/Shader/Shader.hpp"
#include <filesystem>
#include "Core/Application/Utilities/AssetUtilities.hpp"

namespace Beer::Rendering
{
    Shader::Shader(const std::filesystem::path path,
        const Core::Device& device)
    {
        auto shaderCode = Core::AssetUtilities::LoadSpvFile(path);

        SpvReflectShaderModule
    }
} // namespace Beer::Rendering
